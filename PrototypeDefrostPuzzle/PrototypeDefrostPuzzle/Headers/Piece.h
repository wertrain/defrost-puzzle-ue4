#pragma once

#include <cinttypes>

class Piece
{
public:
    Piece();
    ~Piece();

    int32_t GetX() const { return m_PointX; }
    int32_t GetY() const { return m_PointY; }

private:
    int32_t m_PointX;
    int32_t m_PointY;
};
