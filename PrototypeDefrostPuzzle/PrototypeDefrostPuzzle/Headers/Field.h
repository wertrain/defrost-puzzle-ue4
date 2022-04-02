#pragma once

#include <cinttypes>
#include <vector>
#include <functional>
#include <string>

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
    bool CreateFromString(const char* serialized);
    void Destroy();
    void Dump(std::function<void(const CellType**, int, int)> dumper);
    void PutPieces(std::vector<Position>& pieces, int putNum);
    CellType GetCell(int x, int y) const;
    Position GetGoalPosition() const;
    void Serialize(std::string& dist);

private:
    void CreateField(int width, int height);
    void DestroyField();
    void CreateIsland(int islandNum);
    void FillField(const CellType cellType);

private:
    CellType** m_Field;
    int32_t m_Width;
    int32_t m_Height;
    Position m_Goal;
};

} // namespace game
