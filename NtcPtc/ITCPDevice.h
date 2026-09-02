#ifndef ITCPDEVICE_H
#define ITCPDEVICE_H

#include <QObject>
#include <QString>
#include <cstdint>

/**
 * @brief TCP 设备抽象接口 (针对服务端角色重构)
 * 
 * 用于 NTC/PTC 温度传感器或其他 TCP 协议设备
 * 遵循硬件抽象层设计原则
 */
class ITCPDevice : public QObject
{
    Q_OBJECT

public:
    explicit ITCPDevice(QObject* parent = nullptr) : QObject(parent) {}
    
    enum State {
        Stopped = 0,         // 未监听/已停止
        Listening = 1,       // 正在监听
        ClientConnected = 2, // 已有客户端连入
        Error = 3            // 错误状态
    };
    Q_ENUM(State)

    virtual ~ITCPDevice() = default;

    // ========== 连接管理 ==========
    
    /**
     * @brief 启动 TCP 服务器并监听
     * @param address 监听的主机地址（如 "192.168.0.115"）
     * @param port 端口号
     * @return true 如果启动监听成功
     */
    virtual bool startServer(const QString& address, uint16_t port) = 0;
    
    /**
     * @brief 停止监听并断开所有连接
     */
    virtual void stopServer() = 0;
    
    /**
     * @brief 获取当前连接状态
     * @return 连接状态
     */
    virtual State getState() const = 0;

    // ========== 温度读取（应用层接口）==========
    
    /**
     * @brief 读取指定轴内一路 NTC 通道值
     * @param axisIndex 轴索引 (0-1)
     * @param sensorIndex 轴内 NTC 通道索引 (0-3)
     * @return 温度值 (℃)，如果读取失败或未初始化返回 -999.0
     */
    virtual float readNTCTemperature(int axisIndex, int sensorIndex) = 0;

    /**
     * @brief 读取 NTC 模块内指定通道值
     * @param channelIndex 模块内通道索引 (0-7)
     * @return 温度值 (℃)，如果读取失败或未初始化返回 -999.0
     */
    virtual float readNTCChannel(int channelIndex) = 0;

    /**
     * @brief 读取 NTC 模块内指定通道的原始寄存器值
     * @param channelIndex 模块内通道索引 (0-7)
     * @return 原始无符号16位值；无效或未初始化时返回 0xFFFF
     */
    virtual uint16_t readNTCRawChannel(int channelIndex) = 0;

    /**
     * @brief 读取指定轴内一路 PTC 通道值
     * @param axisIndex 轴索引 (0-1)
     * @param sensorIndex 轴内 PTC 通道索引 (0-1)
     * @return 温度值 (℃)，如果读取失败或未初始化返回 -999.0
     */
    virtual float readPTCTemperature(int axisIndex, int sensorIndex) = 0;

    /**
     * @brief 读取 PTC 模块内指定通道值
     * @param channelIndex 模块内通道索引 (0-7)
     * @return 温度值 (℃)，如果读取失败或未初始化返回 -999.0
     */
    virtual float readPTCChannel(int channelIndex) = 0;

    /**
     * @brief 读取 PTC 模块内指定通道的原始寄存器值
     * @param channelIndex 模块内通道索引 (0-7)
     * @return 原始无符号16位值；无效或未初始化时返回 0xFFFF
     */
    virtual uint16_t readPTCRawChannel(int channelIndex) = 0;

signals:
    void stateChanged(State newState);
    void errorOccurred(const QString& error);
    void dataReceived(const QByteArray& data);
};

#endif // ITCPDEVICE_H
