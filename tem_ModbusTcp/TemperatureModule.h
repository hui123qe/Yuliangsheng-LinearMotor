
#ifndef TEMPERATURE_MODULE_H
#define TEMPERATURE_MODULE_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdint>
#include <QMutex>

class TemperatureModule : public QObject
{
    Q_OBJECT

public:
    explicit TemperatureModule(QObject *parent = nullptr);
    ~TemperatureModule();

    // 连接设备
    bool Connect(const QString& ip, int port);

    // 断开连接
    void Disconnect();

    // 获取连接状态
    bool IsConnected() const;

    // 获取指定通道的温度原始数据（4字节十六进制）
    // 通道范围：1-16
    // 返回值：成功返回true，并将温度数据存入data数组
    bool GetTemperatureRaw(int channel, uint8_t data[4]);

    // 获取指定通道的温度（浮点数）
    // 通道范围：1-16
    // 返回值：成功返回true，并将温度值存入temperature
    bool GetTemperature(int channel, float& temperature);
    // 在 public 部分添加以下两个新接口

    // 获取指定通道的阻值原始数据（4字节十六进制）
    // 通道范围：1-16，对应寄存器地址 0xA9 到 0xC7
    // 返回值：成功返回true，并将阻值数据存入data数组
    bool GetResistanceRaw(int channel, uint8_t data[4]);

    // 获取指定通道的阻值（浮点数）
    // 通道范围：1-16
    // 返回值：成功返回true，并将阻值存入resistance
    bool GetResistance(int channel, float& resistance);
    bool motionTem1(float& num);
    bool motionTem2(float& num);
    bool motionResptc1(float& num);
    bool motionResptc2(float& num);
private:
    QTcpSocket *socket;
    bool is_connected;

    // 计算Modbus CRC16
    uint16_t CalculateCRC(const uint8_t* data, int length);

    // 构建Modbus请求包
    QByteArray BuildRequest(int channel);

    // 发送Modbus请求并接收响应
    bool SendRequest(const QByteArray& request, QByteArray& response);

    // 互斥锁保护（如果多线程调用）
    mutable QMutex mutex;

     QByteArray BuildResistanceRequest(int channel);
};

#endif
