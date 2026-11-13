#ifndef GLOBAL_H
#define GLOBAL_H

namespace Global {
// 使用控制器映射后的变量名。
constexpr const char* DoorLockOutputs[] = {
    "Lock1_OUT", "Lock2_OUT", "Lock3_OUT", "Lock4_OUT"
};
constexpr int DoorLockOn = 1;
constexpr int InputNormal = 1;
// 现场急停接线电平相反时改为 true。
constexpr bool EmergencyStopInputInverted = true;
// bufferTemplate/123.prg：0=IO，1=限位/KILL，2=DC采集服务，9=门锁/光幕联锁。
constexpr int Buffers[] = {0, 1, 2, 9};

extern int doorLockState;
extern int lightCurtainState;
extern int emergencyStopState;
}

#endif
