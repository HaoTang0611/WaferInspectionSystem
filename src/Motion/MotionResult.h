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

// Describes the result of a call, not whether physical motion has completed.
struct MotionResult
{
    MotionError error;

    // Original SDK error code for diagnostics; 0 when none is provided.
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
