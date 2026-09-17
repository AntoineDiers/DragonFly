#pragma once

#include "BaseType.h"

namespace dragonfly_msgs::base_types
{
    template<int64_t Min, int64_t Max, uint8_t NBits>
    class Float : public BaseType<NBits>
    {
    public:
        
        static_assert(Max > Min);

        Float(){}
        Float(const double& val) { set(val); }

        double get() const { return Min + (Max - Min) * ((double)this->_val) / BaseType<NBits>::_MaxVal; }
        bool set(double val) 
        { 
            this->_val = std::clamp(BaseType<NBits>::_MaxVal * (val - Min) / (Max - Min) , 0.0, (double)BaseType<NBits>::_MaxVal);
            return val <= Max && val >= Min;
        }

        nlohmann::json _serialiseJson() const
        {
            return get();
        }
    };
}