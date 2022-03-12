#include "Field.h"

Field::Field()
    : m_Field(nullptr)
    , m_Width(0)
    , m_Height(0)
{

}

Field::~Field() 
{
    Destroy();
}

bool Field::Create(const int width, const int height)
{
    CreateField(width, height);

    FillField(CellType::Frozen);
    {
        for (int x = 0; x < m_Width; ++x)
        {
            m_Field[0][x] = CellType::Block;
            m_Field[height - 1][x] = CellType::Block;
        }

        for (int y = 0; y < m_Height; ++y)
        {
            m_Field[y][0] = CellType::Block;
            m_Field[y][width - 1] = CellType::Block;
        }
    }

    return true;
}

void Field::Destroy()
{
    DestroyField();
}

void Field::Dump(std::function<void(const CellType**, int, int)> dumper)
{
    dumper(const_cast<const CellType**>(m_Field), m_Width, m_Height);
}

void Field::CreateField(const int width, const int height)
{
    DestroyField();

    m_Width = width;
    m_Height = height;

    m_Field = new CellType * [m_Height];
    for (int y = 0; y < m_Height; ++y)
    {
        m_Field[y] = new CellType[m_Width];
    }
}

void Field::DestroyField()
{
    if (m_Field)
    {
        for (int y = 0; y < m_Height; ++y)
        {
            delete[] m_Field[y];
        }
        delete[] m_Field;

        m_Field = nullptr;
    }
    m_Width = m_Height = 0;
}

void Field::FillField(const CellType cellType)
{
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            m_Field[y][x] = cellType;
        }
    }
}
