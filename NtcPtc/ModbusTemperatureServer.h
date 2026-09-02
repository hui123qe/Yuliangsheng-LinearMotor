#ifndef MODBUSTEMPERATURESERVER_H
#define MODBUSTEMPERATURESERVER_H

#include "ITCPDevice.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QVector>

/**
 * @brief Modbus TCP 温度服务端实现
 *
 * 网络角色：本机为 TCP Server，硬件设备作为 TCP Client 主动连入。
 *
 * 查询序列（链式状态机，收到回包立即发下一包）：
 *   模块01(PTC) → 模块02(NTC) → 循环
 *
 * 通道与轴关系：
 *   PTC 模块01：轴0 -> 通道0~1；轴1 -> 通道2~3；通道4~7预留。
 *   NTC 模块02：轴0 -> 通道0~3；轴1 -> 通道4~7。
 *
 * 每个模块始终缓存完整的8个寄存器通道值，不合并通道数据。
 */
class ModbusTemperatureServer : public ITCPDevice
{
    Q_OBJECT

public:
    static constexpr int AXIS_COUNT            = 2;
    static constexpr int MODULE_CHANNEL_COUNT  = 8;
    static constexpr int PTC_CHANNELS_PER_AXIS = 2;
    static constexpr int NTC_CHANNELS_PER_AXIS = 4;

    explicit ModbusTemperatureServer(QObject* parent = nullptr);
    ~ModbusTemperatureServer() override;

    bool  startServer(const QString& address, uint16_t port) override;
    void  stopServer() override;
    State getState()  const override;

    // O(1) 读取，直接返回指定轴内通道的缓存值，无任何阻塞
    float readNTCTemperature(int axisIndex, int sensorIndex) override;
    float readPTCTemperature(int axisIndex, int sensorIndex) override;
    float readNTCChannel(int channelIndex) override;
    float readPTCChannel(int channelIndex) override;
    uint16_t readNTCRawChannel(int channelIndex) override;
    uint16_t readPTCRawChannel(int channelIndex) override;

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    // 链式查询
    void       sendNextQuery();
    QByteArray buildQueryFrame(uint8_t moduleAddr) const;

    // 帧解析
    void     processBuffer();
    void     parseFrame(const QByteArray& frame);
    bool     validateCRC(const QByteArray& frame) const;
    uint16_t calculateCRC16(const QByteArray& data) const;

    // NTC: 二分查找 R-T 表 + 线性插值 (0~100°C)
    float convertNTCRawToTemperature(float R) const;
    // PTC: 阈值判断 R <= 570 → T = 70°C
    float convertPTCRawToTemperature(uint16_t raw) const;

    void setState(State newState);

private:
    QTcpServer* m_server;
    QTcpSocket* m_clientSocket;
    State       m_currentState;
    QByteArray  m_receiveBuffer;

    // 链式查询状态
    QVector<uint8_t> m_querySequence;   // 活跃模块地址列表
    int              m_currentQueryIdx; // 当前发送到第几个
    uint8_t          m_waitingForAddr;  // 当前等待回包的模块地址

    // 温度缓存 —— 每个模块完整保留8个通道
    float m_ptcTemperatures[MODULE_CHANNEL_COUNT];
    float m_ntcTemperatures[MODULE_CHANNEL_COUNT];

    // 原始寄存器缓存 —— 用于按十进制显示模块内每个通道的原始值
    uint16_t m_ptcRawValues[MODULE_CHANNEL_COUNT];
    uint16_t m_ntcRawValues[MODULE_CHANNEL_COUNT];
};

#endif // MODBUSTEMPERATURESERVER_H
