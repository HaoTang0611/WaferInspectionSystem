#ifndef WAFER_INSPECTION_I_MOTION_CONTROLLER_H
#define WAFER_INSPECTION_I_MOTION_CONTROLLER_H

#include "MotionResult.h"

// 單軸控制契約；axisId 由控制器對應至硬體。
class IMotionController
{
public:
    virtual ~IMotionController() {}

    // 成功表示命令已接受，不等待到位；目標與實際位置使用相同單位及原點。
    virtual MotionResult MoveAbsolute(int axisId, double targetPosition) = 0;

    // 查詢成功時，position 才有效。
    virtual MotionResult GetActualPosition(int axisId, double& position) = 0;

    // 查詢成功時 isDone 才有效；閒置或停止皆為 true，不保證到位。
    virtual MotionResult IsMotionDone(int axisId, bool& isDone) = 0;

    // 成功表示停止命令已接受；停止曲線由實作決定，非緊急停止。
    virtual MotionResult Stop(int axisId) = 0;
};

#endif // WAFER_INSPECTION_I_MOTION_CONTROLLER_H
