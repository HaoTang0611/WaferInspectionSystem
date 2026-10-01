#ifndef WAFER_INSPECTION_MOTION_RESULT_H
#define WAFER_INSPECTION_MOTION_RESULT_H

enum class MotionError
{
    None,
    NotInitialized,
    InvalidAxis,
    InvalidArgument,
    Busy,
    Unsupported,
    DriverError
};

// 呼叫結果，不代表運動已完成。
struct MotionResult
{
    MotionError error;

    // SDK 原始錯誤碼，未提供時為 0。
    int nativeErrorCode;

    explicit MotionResult(
        MotionError errorValue = MotionError::None,
        int nativeCode = 0)
        : error(errorValue),
          nativeErrorCode(nativeCode)
    {
    }

    bool IsSuccess() const
    {
        return error == MotionError::None;
    }
};

#endif // WAFER_INSPECTION_MOTION_RESULT_H
