#pragma once

#include "BaseType.h"

namespace dragonfly_msgs::base_types
{
    template<typename E>
    class Enum : public BaseType<neededBits((int)E::_COUNT - 1)>
    {
    public:
        
        static_assert((int)E::_COUNT > 0);

        Enum(){}
        Enum(const E& val) { set(val); }

        E get() const { return (E)this->_val; }
        void set(E val) { this->_val = (uint64_t)val; }

        nlohmann::json _serialiseJson() const
        {
            return (int)get();
        }
    };
}