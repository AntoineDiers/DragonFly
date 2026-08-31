#pragma once

#include <array>
#include <cstdint>
#include <cmath>

class LookUpTableInterface
{
public:

    struct Point
    {
        float x;
        float y;
    };

    virtual float getY(float x) const = 0;
    virtual float getX(float y) const = 0;
};



template<uint32_t NPoints>
class LookUpTable : public LookUpTableInterface
{
public:

    LookUpTable(const std::array<LookUpTableInterface::Point, NPoints>& data) : 
        _data(data) {}

    float getY(float x) const
    {
        if(x < _data[0].x)
        {
            return _data[0].y;
        }

        for(uint32_t i = 0; i < NPoints - 1; i++)
        {
            if(x < _data[i+1].x)
            {
                return _data[i].y + (_data[i + 1].y - _data[i].y) * (x - _data[i].x) / (_data[i+1].x - _data[i].x);
            }
        }
        return _data[NPoints - 1].y;
    };

    float getX(float y) const
    {
        if(y < _data[0].y)
        {
            return _data[0].y;
        }

        for(uint32_t i = 0; i < NPoints - 1; i++)
        {
            if(y < _data[i+1].y)
            {
                return _data[i].x + (_data[i + 1].x - _data[i].x) * (y - _data[i].y) / (_data[i+1].y - _data[i].y);
            }
        }
        return _data[NPoints - 1].x;
    }

private:

    std::array<LookUpTableInterface::Point, NPoints> _data;
};