#pragma once

#include "BaseType.h"

namespace dragonfly_msgs::base_types
{
    class Boolean : public BaseType<1>
    {
    public:

        Boolean(){}
        Boolean(const bool& val) { set(val); }

        bool get() const { return this->_val; }
        void set(bool val) { this->_val = val ? 1 : 0; }

        nlohmann::json _serialiseJson() const
        {
            return get();
        }
    };
}