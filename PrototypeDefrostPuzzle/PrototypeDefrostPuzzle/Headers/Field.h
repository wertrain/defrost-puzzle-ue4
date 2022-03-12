#pragma once

#include <cinttypes>
#include <functional>

class Field
{
public:
    enum class CellType : uint8_t
    {
        Frozen,
        Melted,
        Block
    };

public:
    Field();
    ~Field();

    bool Create(int width, int height);
    void Destroy();
    void Dump(std::function<void(const CellType**, int, int)> dumper);

private:
    void CreateField(int width, int height);
    void DestroyField();
    void FillField(const CellType cellType);

private:
    CellType** m_Field;
    int32_t m_Width;
    int32_t m_Height;
};
