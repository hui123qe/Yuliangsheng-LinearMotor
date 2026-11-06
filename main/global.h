#ifndef GLOBAL_H
#define GLOBAL_H

namespace Global {
// 临时 ACS 变量名和电平，现场接线确定后在此替换。
constexpr char DoorLockOutput[] = "DoorLock_OUT";
constexpr char DoorLockInput[] = "DoorLock_IN";
constexpr char LightCurtainInput[] = "LightCurtain_IN";
constexpr int DoorLockOn = 1;
constexpr int InputNormal = 1;
// 现场急停接线电平相反时改为 true。
constexpr bool EmergencyStopInputInverted = true;
// 暂按仓库模板：0=IO，1=限位/KILL，2=采集。
constexpr int Buffers[] = {0, 1, 2};

extern int doorLockState;
extern int lightCurtainState;
extern int emergencyStopState;
extern bool safetyResetRequired;
}

#endif
