
#pragma once

namespace dragonfly_msgs { 
namespace enums { 


enum class ErrorState
{
    TRIGGERED_LESS_THAN_1_S_AGO,
    TRIGGERED_LESS_THAN_30_S_AGO,
    TRIGGERED_MORE_THAN_30_S_AGO,
    NEVER_TRIGGERED,
    _COUNT
};

}}

