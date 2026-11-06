#ifndef GLOBAL_H
#define GLOBAL_H

namespace Global {
// 使用控制器映射后的变量名。
constexpr const char* DoorLockOutputs[] = {
    "Lock1_OUT", "Lock2_OUT", "Lock3_OUT", "Lock4_OUT"
};
constexpr const char* DoorLockInputs[] = {
    "LockOff1_IN", "LockOff2_IN", "LockOff3_IN", "LockOff4_IN"
};
constexpr int DoorLockLocked = 1;
constexpr const char* LightCurtainInputs[] = {"SLC1_IN", "SLC2_IN"};
constexpr int LightCurtainNormal = 0;
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
