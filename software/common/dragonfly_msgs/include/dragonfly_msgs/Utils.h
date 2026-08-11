#pragma once

#include <limits>
#include <stdexcept>
#include <optional>
#include <cstdint>
#include <cstring>
#include <ostream>

#include <dragonfly_utils/Buffer.h>

// Check floating point numbers have the expected size
static_assert(sizeof(float) == 4);
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(sizeof(double) == 8);
static_assert(std::numeric_limits<double>::is_iec559);

namespace dragonfly_msgs
{
    typedef uint8_t MsgId;

    namespace internal
    {
        template<typename T>
        void serialiseField(const T& field, uint8_t* &buffer)
        {
            if constexpr (
                std::is_same_v<T,float> ||
                std::is_same_v<T,double> ||
                std::is_same_v<T, bool>)
            {
                memcpy(buffer, &field, sizeof(T));
                buffer += sizeof(T);
            }

            else if constexpr (
                std::is_same_v<T,uint8_t> ||
                std::is_same_v<T,uint16_t> ||
                std::is_same_v<T,uint32_t> ||
                std::is_same_v<T,uint64_t> )
            {
                for(uint32_t i = 0; i < sizeof(T); i++)
                {
                    *buffer = ( field << (8 * i) ) & 0xFF;
                    buffer += 1;
                }
            }

            else
            {
                field._serialiseFields(buffer);
            }
        }

        template<typename T, uint32_t ArraySize>
        void serialiseArrayField(const T (&field)[ArraySize], uint8_t* &buffer)
        {
            for(uint32_t i = 0; i < ArraySize; i++)
            {
                serialiseField<T>(field[i], buffer);
            }
        }

        template<typename T>
        void deserialiseField(T& field, const uint8_t* &buffer)
        {
            if constexpr (
                std::is_same_v<T,float> ||
                std::is_same_v<T,double> ||
                std::is_same_v<T, bool>)
            {
                memcpy(&field, buffer, sizeof(T));
                buffer += sizeof(T);
            }
            else if constexpr (
                std::is_same_v<T,uint8_t> ||
                std::is_same_v<T,uint16_t> ||
                std::is_same_v<T,uint32_t> ||
                std::is_same_v<T,uint64_t> )
            {
                field = 0;
                for(uint32_t i = 0; i < sizeof(T); i++)
                {
                    field += ((uint64_t)*buffer) << (8 * i);
                    buffer += 1;
                }
            }
            else
            {
                field._deserialiseFields(buffer);
            }
        }

        template<typename T, uint32_t ArraySize>
        void deserialiseArrayField(T (&field)[ArraySize], const uint8_t* &buffer)
        {
            for(uint32_t i = 0; i < ArraySize; i++)
            {
                deserialiseField<T>(field[i], buffer);
            }
        }

        template<typename T>
        void printField(std::ostream& os, const std::string& name, const T& value)
        {
            os << "\"" << name << "\" : ";
            if constexpr (std::is_same_v<uint8_t, T>)
            {
                os << (int)value;
            }
            else
            {
                os << value;
            }
            os << ", ";
        }

        template<typename T, uint32_t ArraySize>
        void printArrayField(std::ostream& os, const std::string& name, const T (&values)[ArraySize])
        {
            os << "\"" << name << "\" : [";
            if constexpr (std::is_same_v<uint8_t, T>)
            {
                for(uint32_t i = 0; i < ArraySize; i++)
                {
                    os << (int)values[i] << ", ";
                }
            }
            else
            {
                for(uint32_t i = 0; i < ArraySize; i++)
                {
                    os << values[i] << ", ";
                }
            }
            os << "], ";
        }
    };
   
};