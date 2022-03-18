#include "Field.h"

#include <random>
#include <iterator>
#include <numeric>

namespace game
{

Field::Field()
    : m_Field(nullptr)
    , m_Width(0)
    , m_Height(0)
    , m_Islands()
    , m_GoalIndex(0)
{

}

Field::~Field() 
{
    Destroy();
}

bool Field::Create(const CreateParameter& param)
{
    CreateField(param.width, param.height);

    // フィールドを初期化
    {
        FillField(CellType::Frozen);

        for (int x = 0; x < m_Width; ++x)
        {
            m_Field[0][x] = CellType::Block;
            m_Field[param.height - 1][x] = CellType::Block;
        }

        for (int y = 0; y < m_Height; ++y)
        {
            m_Field[y][0] = CellType::Block;
            m_Field[y][param.width - 1] = CellType::Block;
        }

        // 各辺に二つずつブロックを置く
        {
            constexpr int offset = 2;
            std::mt19937 mt{ std::random_device{}() };

            auto getr2 = [&](const int base)
            {
                std::uniform_int_distribution<int> distFirst(offset, base / 2);
                const int v1 = distFirst(mt);
                std::uniform_int_distribution<int> distSecond(v1 + 2, base - offset);
                const int v2 = distSecond(mt);
                return std::tuple<int, int>(v1, v2);
            };

            const auto w1 = getr2(m_Width), w2 = getr2(m_Width);
            const auto h1 = getr2(m_Height), h2 = getr2(m_Height);
            m_Field[1][std::get<0>(w1)] = CellType::Block;
            m_Field[1][std::get<1>(w1)] = CellType::Block;
            m_Field[m_Height - 2][std::get<0>(w2)] = CellType::Block;
            m_Field[m_Height - 2][std::get<1>(w2)] = CellType::Block;
            m_Field[std::get<0>(h1)][1] = CellType::Block;
            m_Field[std::get<1>(h1)][1] = CellType::Block;
            m_Field[std::get<0>(h2)][m_Width - 2] = CellType::Block;
            m_Field[std::get<1>(h2)][m_Width - 2] = CellType::Block;
        }
    }

    // 小島を作成
    // （小島とはフィールド上に存在する三つのブロックに囲まれた一点
    // 　例えば Corner::LeftUp のタイプの小島は ■ をブロックとすると
    // 　■■□
    // 　■□□
    // 　□□□
    // 　というような形となり、スタートとゴールはここに配置される）
    CreateIsland(param.level);

    return true;
}

void Field::Destroy()
{
    m_Islands.clear();

    DestroyField();
}

void Field::Dump(std::function<void(const CellType**, int, int)> dumper)
{
    dumper(const_cast<const CellType**>(m_Field), m_Width, m_Height);
}

void Field::PutPieces(std::vector<Position>& pieces, const int putNum)
{
    constexpr int offset = 2;
    std::mt19937 mt{ std::random_device{}() };

    std::uniform_int_distribution<int> distX(offset, m_Width - offset);
    std::uniform_int_distribution<int> distY(offset, m_Height - offset);

    auto getPos = [&]() { return std::tuple<int, int>(distX(mt), distY(mt)); };
    auto checkPos = [&](const int px, const int py)
    {
        if (m_Field[py][px] != CellType::Frozen) { return false; }

        for (int x = px; x < m_Width; ++x)
        {
            if (m_Field[py][x] == CellType::Block) { break; }
            if (m_Field[py][x] == CellType::Goal) { return false; }
        }
        for (int y = py; y < m_Height; ++y)
        {
            if (m_Field[y][px] == CellType::Block) { break; }
            if (m_Field[y][px] == CellType::Goal) { return false; }
        }
        for (int x = px; x >= 0; --x)
        {
            if (m_Field[py][x] == CellType::Block) { break; }
            if (m_Field[py][x] == CellType::Goal) { return false; }
        }
        for (int y = py; y >= 0; --y)
        {
            if (m_Field[y][px] == CellType::Block) { break; }
            if (m_Field[y][px] == CellType::Goal) { return false; }
        }
        return true;
    };

    pieces.clear();

    auto count = putNum;
    while (count > 0)
    {
        auto pos = getPos();

        const int px = std::get<0>(pos), py = std::get<1>(pos);
        if (checkPos(px, py))
        {
            m_Field[py][px] = CellType::Piece;

            pieces.push_back(Field::Position(px, py));
            --count;
        }
    }
}

Field::CellType Field::GetCell(const int x, const int y) const
{
    _ASSERT(x < m_Width&& y < m_Height);

    return m_Field[y][x];
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

void Field::CreateIsland(const int islandNum)
{
    constexpr int offset = 2;
    std::mt19937 mt{ std::random_device{}() };

    std::uniform_int_distribution<int> distX(offset, m_Width - offset);
    std::uniform_int_distribution<int> distY(offset, m_Height - offset);

    // 指定されたインデックスの一定範囲のブロックをチェックする
    auto checkArea = [this](int cx, int cy)
    {
        constexpr int area = 3;

        for (int y = cy - area; y < cy + area; ++y)
        {
            if (y < 0 || y > m_Height - 1)
            {
                return false;
            }

            for (int x = cx - area; x < cx + area; ++x)
            {
                if (x < 0 || x > m_Width - 1)
                {
                    return false;
                }

                if (m_Field[y][x] != CellType::Frozen)
                {
                    return false;
                }
            }
        }
        return true;
    };

    int level = islandNum;
    while (level > 0)
    {
        const int tempX = distX(mt), tempY = distY(mt);

        if (checkArea(tempX, tempY))
        {
            --level;

            std::uniform_int_distribution<int> distDir(0, 3);
            const auto corner = static_cast<Island::Corner>(distDir(mt));
            switch (corner)
            {
            case Island::Corner::LeftUp:
                m_Field[tempY][tempX - 1] = CellType::Block;
                m_Field[tempY - 1][tempX - 1] = CellType::Block;
                m_Field[tempY - 1][tempX] = CellType::Block;
                break;
            case Island::Corner::RightUp:
                m_Field[tempY - 1][tempX] = CellType::Block;
                m_Field[tempY - 1][tempX + 1] = CellType::Block;
                m_Field[tempY][tempX + 1] = CellType::Block;
                break;
            case Island::Corner::LeftBottom:
                m_Field[tempY][tempX - 1] = CellType::Block;
                m_Field[tempY + 1][tempX - 1] = CellType::Block;
                m_Field[tempY + 1][tempX] = CellType::Block;
                break;
            case Island::Corner::RightBottom:
                m_Field[tempY][tempX + 1] = CellType::Block;
                m_Field[tempY + 1][tempX + 1] = CellType::Block;
                m_Field[tempY + 1][tempX] = CellType::Block;
                break;
            }

            const Island island(tempX, tempY, corner);
            m_Islands.push_back(island);
        }
    }

    // 小島からスタート＆ゴールを決定
    {
        std::vector<int> v(4);
        std::iota(v.begin(), v.end(), 0);
        std::shuffle(v.begin(), v.end(), mt);

        m_GoalIndex = v[0];
        m_Field[m_Islands[m_GoalIndex].y][m_Islands[m_GoalIndex].x] = CellType::Goal;
    }
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

} // namespace game
