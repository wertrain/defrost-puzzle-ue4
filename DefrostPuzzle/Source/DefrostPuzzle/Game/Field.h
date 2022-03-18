#pragma once

#include <cinttypes>
#include <vector>
#include <functional>

namespace game
{

class Field
{
public:
    enum class CellType : uint8_t
    {
        Frozen,
        HardFrozen,
        Melted,
        Block,
        Piece,
        Goal,
    };

    struct CreateParameter
    {
        int width;
        int height;
        int level;

        CreateParameter()
            : width(20)
            , height(20)
            , level(6)
        {

        }
    };

    struct Position
    {
        int x;
        int y;

        Position(int x, int y)
            : x(x)
            , y(y)
        {}
    };

public:
    Field();
    ~Field();

    bool Create(const CreateParameter& param);
    void Destroy();
    void Dump(std::function<void(const CellType**, int, int)> dumper);
    void PutPieces(std::vector<Position>& pieces, int putNum);
    CellType GetCell(int x, int y) const;

private:
    void CreateField(int width, int height);
    void DestroyField();
    void CreateIsland(int islandNum);
    void FillField(const CellType cellType);

private:
    struct Island : public Position
    {
        enum class Corner : uint8_t
        {
            LeftUp,
            RightUp,
            LeftBottom,
            RightBottom,
        };
        Corner corner;

        Island(int x, int y, Corner corner)
            : Position(x, y)
            , corner(corner)
        {}
    };

private:
    CellType** m_Field;
    int32_t m_Width;
    int32_t m_Height;
    std::vector<Island> m_Islands;
    int32_t m_GoalIndex;
};

} // namespace game
