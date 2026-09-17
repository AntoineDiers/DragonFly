#pragma once

#include "BaseType.h"

namespace dragonfly_msgs::base_types
{
    template<int64_t Min, int64_t Max>
    class Int : public BaseType<neededBits(Max - Min)>
    {
    public:

        static_assert(Max > Min);

        Int(){}
        Int(const int64_t& val) { set(val); }

        int64_t get() const { return Min + this->_val; }
        bool set(const int64_t& val) 
        { 
            this->_val = std::clamp(val - Min, (int64_t)0, Max - Min);
            return val <= Max && val >= Min;
        }

        nlohmann::json _serialiseJson() const
        {
            return get();
        }
    };
}