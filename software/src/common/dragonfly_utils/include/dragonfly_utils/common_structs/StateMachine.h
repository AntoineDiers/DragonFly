#pragma once

enum ControlModes
{
    IDLE = 0,
    MANUAL,
    AUTO// not implemented yet
};

enum FallbackModes
{
    NONE = 0,
    CLOSED_LOOP_MANUAL,
    OPEN_LOOP_MANUAL,
    CLOSED_LOOP_ORBIT,
    CLOSED_LOOP_DESCENT,
    CLOSED_LOOP_LANDING,
    OPEN_LOOP_GLIDE,
    OPEN_LOOP_DESCENT,
    OPEN_LOOP_LANDING,
};

