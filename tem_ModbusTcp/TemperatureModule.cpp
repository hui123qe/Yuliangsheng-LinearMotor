
#include "TemperatureModule.h"
#include <QDebug>

TemperatureModule::TemperatureModule(QObject *parent)
    : QObject(parent)
    , socket(nullptr)
    , is_connected(false)
{
}

TemperatureModule::~TemperatureModule()
{
    Disconnect();
}

uint16_t TemperatureModule::CalculateCRC(const uint8_t* data, int length)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

QByteArray TemperatureModule::BuildRequest(int channel)
{
    QByteArray request;
    request.resize(8);

    // 通道1: 0x19, 通道2: 0x1B, 通道3: 0x1D, ...
    uint16_t reg_addr = 0x0019 + (channel - 1) * 2;

    uint8_t* data = reinterpret_cast<uint8_t*>(request.data());
    data[0] = 0x01;  // 设备地址
    data[1] = 0x03;  // 功能码
    data[2] = (reg_addr >> 8) & 0xFF;
    data[3] = reg_addr & 0xFF;
    data[4] = 0x00;
    data[5] = 0x02;

    // 计算CRC
    uint16_t crc = CalculateCRC(data, 6);
    data[6] = crc & 0xFF;
    data[7] = (crc >> 8) & 0xFF;

    return request;
}

bool TemperatureModule::SendRequest(const QByteArray& request, QByteArray& response)
{
    QMutexLocker locker(&mutex);

    if (!is_connected || !socket || socket->state() != QAbstractSocket::ConnectedState) {
        return false;
    }

    // 清空残留数据
    while (socket->bytesAvailable() > 0) {
        socket->readAll();
    }

    // 发送请求
    qint64 sent = socket->write(request);
    if (sent != request.size()) {
        return false;
    }

    // 等待响应（最多500ms）
    if (!socket->waitForReadyRead(500)) {
        return false;
    }

    response = socket->readAll();

    // 验证响应长度（Modbus RTU: 地址+功能码+数据长度+4字节数据+CRC = 9字节）
    if (response.size() != 9) {
        return false;
    }

    // 验证响应
    const uint8_t* resp_data = reinterpret_cast<const uint8_t*>(response.constData());
    if (resp_data[0] != 0x01 || resp_data[1] != 0x03) {
        return false;
    }

    // 验证CRC
    uint16_t calc_crc = CalculateCRC(resp_data, response.size() - 2);
    uint16_t resp_crc = resp_data[response.size() - 2] | (resp_data[response.size() - 1] << 8);
    if (calc_crc != resp_crc) {
        return false;
    }

    return true;
}

bool TemperatureModule::Connect(const QString& ip, int port)
{
    QMutexLocker locker(&mutex);

    if (is_connected) {
        Disconnect();
    }

    // 创建socket
    if (!socket) {
        socket = new QTcpSocket(this);
    }

    // 连接到服务器
    socket->connectToHost(ip, port);

    // 等待连接完成（超时1秒）
    if (!socket->waitForConnected(1000)) {
        return false;
    }

    // 设置socket选项
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);  // 禁用Nagle算法

    is_connected = true;

    return true;
}

void TemperatureModule::Disconnect()
{
    QMutexLocker locker(&mutex);

    if (socket) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->disconnectFromHost();
            socket->waitForDisconnected(500);
        }
        socket->deleteLater();
        socket = nullptr;
    }
    is_connected = false;
}

bool TemperatureModule::IsConnected() const
{
    QMutexLocker locker(&mutex);
    return is_connected && socket && socket->state() == QAbstractSocket::ConnectedState;
}

bool TemperatureModule::GetTemperatureRaw(int channel, uint8_t data[4])
{
    if (!IsConnected()) {
        return false;
    }

    if (channel < 1 || channel > 16) {
        return false;
    }

    // 构建请求
    QByteArray request = BuildRequest(channel);

    // 发送请求并接收响应
    QByteArray response;
    if (!SendRequest(request, response)) {
        return false;
    }

    // 提取温度数据（4字节）
    const uint8_t* resp_data = reinterpret_cast<const uint8_t*>(response.constData());
    memcpy(data, resp_data + 3, 4);

    return true;
}

bool TemperatureModule::GetTemperature(int channel, float& temperature)
{
    uint8_t tempData[4];
    if (!GetTemperatureRaw(channel, tempData)) {
        return false;
    }

    memcpy(&temperature, tempData, sizeof(float));
    return true;
}


// 添加新的 BuildResistanceRequest 函数（私有，需要在头文件中声明）
// 或者复用 BuildRequest 但增加参数，为了清晰，我单独写一个函数

QByteArray TemperatureModule::BuildResistanceRequest(int channel)
{
    QByteArray request;
    request.resize(8);

    // 阻值通道1: 0x39, 通道2: 0x3B, 通道3: 0x3D, 通道4: 0x3F, 通道5: 0x41, 通道6: 0x43
    // 0x39 = 57, 每次增加2
    uint16_t reg_addr = 0x0039 + (channel - 1) * 2;

    uint8_t* data = reinterpret_cast<uint8_t*>(request.data());
    data[0] = 0x01;  // 设备地址
    data[1] = 0x03;  // 功能码
    data[2] = (reg_addr >> 8) & 0xFF;
    data[3] = reg_addr & 0xFF;
    data[4] = 0x00;
    data[5] = 0x02;

    // 计算CRC
    uint16_t crc = CalculateCRC(data, 6);
    data[6] = crc & 0xFF;
    data[7] = (crc >> 8) & 0xFF;

    return request;
}

bool TemperatureModule::GetResistanceRaw(int channel, uint8_t data[4])
{
    if (!IsConnected()) {
        return false;
    }

    if (channel < 1 || channel > 16) {
        return false;
    }

    // 构建阻值请求
    QByteArray request = BuildResistanceRequest(channel);

    // 发送请求并接收响应
    QByteArray response;
    if (!SendRequest(request, response)) {
        return false;
    }

    // 提取阻值数据（4字节）
    const uint8_t* resp_data = reinterpret_cast<const uint8_t*>(response.constData());
    memcpy(data, resp_data + 3, 4);

    return true;
}

bool TemperatureModule::GetResistance(int channel, float& resistance)
{
    uint8_t resistanceData[4];
    if (!GetResistanceRaw(channel, resistanceData)) {
        return false;
    }

    memcpy(&resistance, resistanceData, sizeof(float));
    return true;
}

bool TemperatureModule::motionTem1(float& num)
{
    if (!IsConnected()) {
        return false;
    }

    float temperatures[4];
    int validCount = 0;

    // 获取通道1-4的温度
    for (int channel = 1; channel <= 4; channel++) {
        if (GetTemperature(channel, temperatures[channel - 1])) {
            validCount++;
        } else {
            temperatures[channel - 1] = -9999.0f; // 无效值
        }
    }

    if (validCount == 0) {
        return false;
    }

    // 找出最大值
    num = temperatures[0];
    for (int i = 1; i < 4; i++) {
        if (temperatures[i] > num) {
            num = temperatures[i];
        }
    }

    return true;
}

bool TemperatureModule::motionTem2(float& num)
{
    if (!IsConnected()) {
        return false;
    }

    float temperatures[4];
    int validCount = 0;

    // 获取通道7-10的温度
    for (int channel = 7; channel <= 10; channel++) {
        if (GetTemperature(channel, temperatures[channel - 7])) {
            validCount++;
        } else {
            temperatures[channel - 7] = -9999.0f;
        }
    }

    if (validCount == 0) {
        return false;
    }

    // 找出最大值
    num = temperatures[0];
    for (int i = 1; i < 4; i++) {
        if (temperatures[i] > num) {
            num = temperatures[i];
        }
    }

    return true;
}

bool TemperatureModule::motionResptc1(float& num)
{
    if (!IsConnected()) {
        return false;
    }

    float resistances[2];
    int validCount = 0;

    // 获取通道5-6的阻值
    for (int channel = 5; channel <= 6; channel++) {
        if (GetResistance(channel, resistances[channel - 5])) {
            validCount++;
        } else {
            resistances[channel - 5] = -9999.0f;
        }
    }

    if (validCount == 0) {
        return false;
    }

    // 找出最大值
    num = resistances[0];
    for (int i = 1; i < 2; i++) {
        if (resistances[i] > num) {
            num = resistances[i];
        }
    }

    return true;
}

bool TemperatureModule::motionResptc2(float& num)
{
    if (!IsConnected()) {
        return false;
    }

    float resistances[2];
    int validCount = 0;

    // 获取通道11-12的阻值
    for (int channel = 11; channel <= 12; channel++) {
        if (GetResistance(channel, resistances[channel - 11])) {
            validCount++;
        } else {
            resistances[channel - 11] = -9999.0f;
        }
    }

    if (validCount == 0) {
        return false;
    }

    // 找出最大值
    num = resistances[0];
    for (int i = 1; i < 2; i++) {
        if (resistances[i] > num) {
            num = resistances[i];
        }
    }

    return true;
}
