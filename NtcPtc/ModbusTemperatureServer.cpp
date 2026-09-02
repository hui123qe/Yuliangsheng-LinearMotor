#include "ModbusTemperatureServer.h"
#include <QHostAddress>
#include <QDebug>

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------
ModbusTemperatureServer::ModbusTemperatureServer(QObject* parent)
    : ITCPDevice(parent),
      m_server(new QTcpServer(this)),
      m_clientSocket(nullptr),
      m_currentState(Stopped),
      m_currentQueryIdx(0),
      m_waitingForAddr(0)
{
    // 初始化缓存为无效值
    for (int i = 0; i < MODULE_CHANNEL_COUNT; ++i) {
        m_ptcTemperatures[i] = -999.0f;
        m_ntcTemperatures[i] = -999.0f;
        m_ptcRawValues[i] = 0xFFFF;
        m_ntcRawValues[i] = 0xFFFF;
    }

    // 活跃查询序列：模块01为PTC，模块02为NTC。
    m_querySequence = {0x01, 0x02};

    connect(m_server, &QTcpServer::newConnection,
            this, &ModbusTemperatureServer::onNewConnection);
}

ModbusTemperatureServer::~ModbusTemperatureServer()
{
    stopServer();
}

// ---------------------------------------------------------------------------
// 服务端管理
// ---------------------------------------------------------------------------
bool ModbusTemperatureServer::startServer(const QString& address, uint16_t port)
{
    if (m_server->isListening()) {
        stopServer();
    }

    QHostAddress hostAddr(address);
    if (!m_server->listen(hostAddr, port)) {
        emit errorOccurred(
            QString("Failed to listen on %1:%2 - %3")
                .arg(address).arg(port).arg(m_server->errorString()));
        return false;
    }

    setState(Listening);
    return true;
}

void ModbusTemperatureServer::stopServer()
{
    if (m_clientSocket) {
        m_clientSocket->disconnectFromHost();
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
    if (m_server->isListening()) {
        m_server->close();
    }
    m_waitingForAddr = 0;
    setState(Stopped);
}

ITCPDevice::State ModbusTemperatureServer::getState() const
{
    return m_currentState;
}

// ---------------------------------------------------------------------------
// 应用层 O(1) 读取接口
// ---------------------------------------------------------------------------
float ModbusTemperatureServer::readNTCTemperature(int axisIndex, int sensorIndex)
{
    if (axisIndex >= 0 && axisIndex < AXIS_COUNT &&
        sensorIndex >= 0 && sensorIndex < NTC_CHANNELS_PER_AXIS) {
        const int channelIndex = axisIndex * NTC_CHANNELS_PER_AXIS + sensorIndex;
        return readNTCChannel(channelIndex);
    }
    return -999.0f;
}

float ModbusTemperatureServer::readPTCTemperature(int axisIndex, int sensorIndex)
{
    if (axisIndex >= 0 && axisIndex < AXIS_COUNT &&
        sensorIndex >= 0 && sensorIndex < PTC_CHANNELS_PER_AXIS) {
        const int channelIndex = axisIndex * PTC_CHANNELS_PER_AXIS + sensorIndex;
        return readPTCChannel(channelIndex);
    }
    return -999.0f;
}

float ModbusTemperatureServer::readNTCChannel(int channelIndex)
{
    if (channelIndex >= 0 && channelIndex < MODULE_CHANNEL_COUNT)
        return m_ntcTemperatures[channelIndex];
    return -999.0f;
}

float ModbusTemperatureServer::readPTCChannel(int channelIndex)
{
    if (channelIndex >= 0 && channelIndex < MODULE_CHANNEL_COUNT)
        return m_ptcTemperatures[channelIndex];
    return -999.0f;
}

uint16_t ModbusTemperatureServer::readNTCRawChannel(int channelIndex)
{
    if (channelIndex >= 0 && channelIndex < MODULE_CHANNEL_COUNT)
        return m_ntcRawValues[channelIndex];
    return 0xFFFF;
}

uint16_t ModbusTemperatureServer::readPTCRawChannel(int channelIndex)
{
    if (channelIndex >= 0 && channelIndex < MODULE_CHANNEL_COUNT)
        return m_ptcRawValues[channelIndex];
    return 0xFFFF;
}

// ---------------------------------------------------------------------------
// 网络事件槽
// ---------------------------------------------------------------------------
void ModbusTemperatureServer::onNewConnection()
{
    if (!m_server->hasPendingConnections()) return;

    // 若已有旧连接，先断开（只保持单一设备连接）
    if (m_clientSocket) {
        m_clientSocket->disconnect(this);
        m_clientSocket->disconnectFromHost();
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }

    m_clientSocket = m_server->nextPendingConnection();
    connect(m_clientSocket, &QTcpSocket::disconnected,
            this, &ModbusTemperatureServer::onClientDisconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead,
            this, &ModbusTemperatureServer::onReadyRead);

    m_receiveBuffer.clear();
    m_currentQueryIdx = 0;
    setState(ClientConnected);

    // ★ 设备连入后立即发出第一条查询，启动链式循环
    sendNextQuery();
}

void ModbusTemperatureServer::onClientDisconnected()
{
    if (m_clientSocket) {
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
    m_waitingForAddr = 0;
    m_receiveBuffer.clear();
    setState(Listening);
}

void ModbusTemperatureServer::onReadyRead()
{
    if (!m_clientSocket) return;

    // 追加到接收缓冲区，处理粘包/半包
    m_receiveBuffer.append(m_clientSocket->readAll());
    emit dataReceived(m_receiveBuffer);

    processBuffer();
}

// ---------------------------------------------------------------------------
// 帧处理：从缓冲区中提取完整帧
// ---------------------------------------------------------------------------
void ModbusTemperatureServer::processBuffer()
{
    while (m_receiveBuffer.size() >= 5) {
        quint8 addr    = static_cast<quint8>(m_receiveBuffer.at(0));
        quint8 funcCode = static_cast<quint8>(m_receiveBuffer.at(1));

        int totalSize = 0;

        if (funcCode == 0x03) {
            // 正常响应：[Addr][03][ByteCount][Data...][CRC_Lo][CRC_Hi]
            quint8 byteCount = static_cast<quint8>(m_receiveBuffer.at(2));
            totalSize = 3 + byteCount + 2;
        } else if (funcCode == 0x83) {
            // 错误响应：[Addr][83][ExceptionCode][CRC_Lo][CRC_Hi]
            totalSize = 5;
        } else {
            // 未知帧头，丢弃一字节重新同步
            qWarning() << "[Parser] Unknown function code:"
                       << QString("0x%1").arg(funcCode, 2, 16, QChar('0')).toUpper()
                       << " - discarding 1 byte";
            m_receiveBuffer.remove(0, 1);
            continue;
        }

        if (m_receiveBuffer.size() < totalSize) {
            // 数据尚不完整，等待更多数据到达
            break;
        }

        // 取出完整一帧
        QByteArray frame = m_receiveBuffer.left(totalSize);
        m_receiveBuffer.remove(0, totalSize);

        // 地址校验
        if (addr != m_waitingForAddr) {
            qWarning() << "[Parser] Stale/unexpected response: expected"
                       << QString("0x%1").arg(m_waitingForAddr, 2, 16, QChar('0')).toUpper()
                       << "got"
                       << QString("0x%1").arg(addr, 2, 16, QChar('0')).toUpper()
                       << "- parsing data but NOT advancing query chain.";

            // 解析残留包中的数据（数据本身是有效的，存入缓存）
            parseFrame(frame);

            // ★ 关键：不调用 sendNextQuery()！
            // 设备仍会响应我们已发出的那条真实查询（m_waitingForAddr），
            // 等那条正确回包到来后再推进链，避免永久错位。
            continue;
        }

        parseFrame(frame);

        // ★ 地址匹配：收到期望的回包，立即发下一条查询（链式驱动核心）
        sendNextQuery();
    }
}

// ---------------------------------------------------------------------------
// 解析单帧数据
// ---------------------------------------------------------------------------
void ModbusTemperatureServer::parseFrame(const QByteArray& frame)
{
    if (!validateCRC(frame)) {
        emit errorOccurred(
            QString("[Module 0x%1] CRC validation failed, raw: %2")
                .arg(static_cast<quint8>(frame.at(0)), 2, 16, QChar('0')).toUpper()
                .arg(QString(frame.toHex(' ').toUpper())));
        return;
    }

    quint8 addr     = static_cast<quint8>(frame.at(0));
    quint8 funcCode = static_cast<quint8>(frame.at(1));

    if (funcCode == 0x83) {
        quint8 exCode = static_cast<quint8>(frame.at(2));
        emit errorOccurred(
            QString("[Module 0x%1] Modbus exception, code: 0x%2")
                .arg(addr, 2, 16, QChar('0')).toUpper()
                .arg(exCode, 2, 16, QChar('0')).toUpper());
        return;
    }

    // 正常响应：解析并缓存完整的8个寄存器通道。
    quint8 byteCount = static_cast<quint8>(frame.at(2));
    int numRegisters = byteCount / 2;

    bool isPTC = (addr == 0x01);
    bool isNTC = (addr == 0x02);

    for (int ch = 0; ch < numRegisters && ch < 8; ++ch) {
        quint8   high = static_cast<quint8>(frame.at(3 + ch * 2));
        quint8   low  = static_cast<quint8>(frame.at(3 + ch * 2 + 1));
        uint16_t raw  = static_cast<uint16_t>((high << 8) | low);

        if (isPTC) {
            m_ptcRawValues[ch] = raw;
            m_ptcTemperatures[ch] =
                (raw == 0xFFFF) ? -999.0f : convertPTCRawToTemperature(raw);
        } else if (isNTC) {
            m_ntcRawValues[ch] = raw;
            m_ntcTemperatures[ch] =
                (raw == 0xFFFF) ? -999.0f : convertNTCRawToTemperature(static_cast<float>(raw));
        }
    }
}

// ---------------------------------------------------------------------------
// 链式查询：发送当前模块查询帧，然后推进索引
// ---------------------------------------------------------------------------
void ModbusTemperatureServer::sendNextQuery()
{
    if (!m_clientSocket ||
        m_clientSocket->state() != QAbstractSocket::ConnectedState) return;

    if (m_querySequence.isEmpty()) return;

    uint8_t addr = m_querySequence.at(m_currentQueryIdx);
    m_waitingForAddr = addr;

    QByteArray frame = buildQueryFrame(addr);
    m_clientSocket->write(frame);
    m_clientSocket->flush();

    // 推进到下一个，循环
    m_currentQueryIdx = (m_currentQueryIdx + 1) % m_querySequence.size();
}

// ---------------------------------------------------------------------------
// 组帧：[Addr][03][00 00][00 08][CRC_Lo][CRC_Hi]
// ---------------------------------------------------------------------------
QByteArray ModbusTemperatureServer::buildQueryFrame(uint8_t moduleAddr) const
{
    QByteArray payload;
    payload.append(static_cast<char>(moduleAddr));
    payload.append(static_cast<char>(0x03));
    payload.append(static_cast<char>(0x00));
    payload.append(static_cast<char>(0x00));
    payload.append(static_cast<char>(0x00));
    payload.append(static_cast<char>(0x08));

    uint16_t crc = calculateCRC16(payload);
    payload.append(static_cast<char>(crc & 0xFF));        // CRC 低字节（小端）
    payload.append(static_cast<char>((crc >> 8) & 0xFF)); // CRC 高字节（小端）

    return payload;
}

// ---------------------------------------------------------------------------
// CRC-16/MODBUS
// ---------------------------------------------------------------------------
bool ModbusTemperatureServer::validateCRC(const QByteArray& frame) const
{
    int len = frame.size();
    if (len < 3) return false;

    QByteArray payload = frame.left(len - 2);
    uint16_t calcCrc   = calculateCRC16(payload);
    uint16_t rcvCrc    = static_cast<quint8>(frame.at(len - 2))
                       | (static_cast<quint8>(frame.at(len - 1)) << 8);
    return calcCrc == rcvCrc;
}

uint16_t ModbusTemperatureServer::calculateCRC16(const QByteArray& data) const
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<uint8_t>(data.at(i));
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// ---------------------------------------------------------------------------
// NTC R-T 查找表（内嵌静态数据，只初始化一次）
// R 降序（高温 = 低阻抗），T 升序
// ---------------------------------------------------------------------------
static const float NTC_R[] = {
    32650.0f, 31027.0f, 29495.0f, 28048.0f, 26680.0f, 25388.0f, 24165.0f, 23009.0f, 21916.0f, 20880.0f,
    19900.0f, 18969.0f, 18087.0f, 17251.0f, 16459.0f, 15708.0f, 14995.0f, 14319.0f, 13678.0f, 13068.0f,
    12490.0f, 11940.0f, 11418.0f, 10921.0f, 10449.0f, 10000.0f, 9572.1f,  9164.9f,  8777.4f,  8408.4f,
    8057.0f,  7721.8f,  7402.5f,  7098.1f,  6808.0f,  6531.3f,  6267.4f,  6015.6f,  5775.4f,  5546.0f,
    5327.0f,  5117.5f,  4917.3f,  4726.1f,  4543.4f,  4368.7f,  4201.7f,  4041.9f,  3889.1f,  3742.9f,
    3603.0f,  3468.7f,  3340.2f,  3217.1f,  3099.2f,  2986.2f,  2878.0f,  2774.3f,  2674.8f,  2579.5f,
    2448.0f,  2400.2f,  2316.0f,  2235.2f,  2157.6f,  2083.0f,  2011.4f,  1942.7f,  1876.6f,  1813.1f,
    1752.0f,  1693.6f,  1637.4f,  1583.4f,  1531.4f,  1481.4f,  1433.2f,  1386.9f,  1342.3f,  1299.4f,
    1258.0f,  1218.1f,  1179.6f,  1142.5f,  1106.8f,  1072.3f,  1039.1f,  1007.1f,  976.23f,  946.44f,
    917.70f,  890.01f,  863.28f,  837.48f,  812.57f,  788.52f,  765.29f,  742.85f,  721.18f,  700.24f,
    680.00f
};

static const float NTC_T[] = {
    0.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,
    10.0f,  11.0f,  12.0f,  13.0f,  14.0f,  15.0f,  16.0f,  17.0f,  18.0f,  19.0f,
    20.0f,  21.0f,  22.0f,  23.0f,  24.0f,  25.0f,  26.0f,  27.0f,  28.0f,  29.0f,
    30.0f,  31.0f,  32.0f,  33.0f,  34.0f,  35.0f,  36.0f,  37.0f,  38.0f,  39.0f,
    40.0f,  41.0f,  42.0f,  43.0f,  44.0f,  45.0f,  46.0f,  47.0f,  48.0f,  49.0f,
    50.0f,  51.0f,  52.0f,  53.0f,  54.0f,  55.0f,  56.0f,  57.0f,  58.0f,  59.0f,
    60.0f,  61.0f,  62.0f,  63.0f,  64.0f,  65.0f,  66.0f,  67.0f,  68.0f,  69.0f,
    70.0f,  71.0f,  72.0f,  73.0f,  74.0f,  75.0f,  76.0f,  77.0f,  78.0f,  79.0f,
    80.0f,  81.0f,  82.0f,  83.0f,  84.0f,  85.0f,  86.0f,  87.0f,  88.0f,  89.0f,
    90.0f,  91.0f,  92.0f,  93.0f,  94.0f,  95.0f,  96.0f,  97.0f,  98.0f,  99.0f,
    100.0f
};

static const int NTC_TABLE_SIZE = static_cast<int>(sizeof(NTC_R) / sizeof(NTC_R[0]));


#if 1
// ---------------------------------------------------------------------------
// NTC 转换：二分查找（R 降序）+ 线性插值
// 输入 R 单位：欧姆（与 R-T 表一致）
// ---------------------------------------------------------------------------
float ModbusTemperatureServer::convertNTCRawToTemperature(float R) const
{
    float temp = R / 100.0f;
    if(temp <0 || temp >200.0f)
        return -999.0f;
    return temp;
}
#else
// ---------------------------------------------------------------------------
// NTC 转换：二分查找（R 降序）+ 最近邻查表
// 返回 R-T 表中距离最近的精确档位温度值（不做插值）
// ---------------------------------------------------------------------------
float ModbusTemperatureServer::convertNTCRawToTemperature(float R) const
{
    // 特殊边界：R=0（设备未接入或异常），默认返回 0°C
    if (R <= 0.0f) return 0.0f;

    // 超过表上限（R > 32650）：温度对应 0°C，直接返回 0
    if (R >= NTC_R[0]) return 0.0f;

    // 超过表下限（R < 680）：温度钳位到表最大值 100°C
    if (R <= NTC_R[NTC_TABLE_SIZE - 1]) return NTC_T[NTC_TABLE_SIZE - 1];

    // 二分查找：找到 NTC_R[lo] > R >= NTC_R[hi] 的相邻区间
    int lo = 0, hi = NTC_TABLE_SIZE - 1;
    while (lo < hi - 1) {
        int mid = (lo + hi) / 2;
        if (NTC_R[mid] > R) lo = mid;
        else                hi = mid;
    }

    // 最近邻：比较 R 到两个相邻档位的距离，返回更近的那个温度
    float dLo = NTC_R[lo] - R;
    float dHi = R - NTC_R[hi];
    return (dLo <= dHi) ? NTC_T[lo] : NTC_T[hi];
}
#endif

// ---------------------------------------------------------------------------
// PTC 转换：阈值判断
// R <= 570 → 正常工况，温度值为 70°C，返回 70.0f
// R >  570 → 温度异常或过温，返回 -999.0f 作为告警标志
// ---------------------------------------------------------------------------
float ModbusTemperatureServer::convertPTCRawToTemperature(uint16_t raw) const
{
    return (raw <= 570) ? 70.0f : -999.0f;
}

// ---------------------------------------------------------------------------
// 状态管理
// ---------------------------------------------------------------------------
void ModbusTemperatureServer::setState(State newState)
{
    if (m_currentState != newState) {
        m_currentState = newState;
        emit stateChanged(m_currentState);
    }
}
