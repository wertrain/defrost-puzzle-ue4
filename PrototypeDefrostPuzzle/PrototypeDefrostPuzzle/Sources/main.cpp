#include <iostream>
#include "Field.h"
#include "Piece.h"

namespace
{

void Dump(const game::Field::CellType** cells, const int width, const int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const char* cell = nullptr;
            switch (cells[y][x])
            {
            case game::Field::CellType::Frozen:
                cell = " ";
                break;
            case game::Field::CellType::Block:
                cell = "¡";
                break;
            case game::Field::CellType::Piece:
                cell = "›";
                break;
            case game::Field::CellType::Goal:
                cell = "";
                break;
            default:
                cell = "@";
                break;
            }
            std::cout << cell;
        }
        std::cout << std::endl;
    }
}

}

int main()
{
    auto field = std::make_unique<game::Field>();

    game::Field::CreateParameter param;
    field->Create(param);

    std::vector<game::Field::Position> positions;
    field->PutPieces(positions, 4);

    std::vector<Piece> pieces;
    for (auto& it : positions)
    {
        pieces.push_back(Piece(it.x, it.y));
    }

    field->Dump(Dump);

    std::string serialized;
    field->Serialize(serialized);

    std::cout << serialized << std::endl;

    field->CreateFromString(serialized.c_str());
    field->Dump(Dump);

    return 0;
}
