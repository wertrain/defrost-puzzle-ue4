#include <iostream>
#include "Field.h"
#include "Piece.h"

namespace
{

void Dump(const Field::CellType** cells, const int width, const int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const char* cell = nullptr;
            switch (cells[y][x])
            {
            case Field::CellType::Frozen:
                cell = " ";
                break;
            case Field::CellType::Block:
                cell = "¡";
                break;
            case Field::CellType::Piece:
                cell = "›";
                break;
            case Field::CellType::Goal:
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
    auto field = std::make_unique<Field>();

    Field::CreateParameter param;
    field->Create(param);
    
    std::vector<Field::Position> positions;
    field->PutPieces(positions, 4);

    std::vector<Piece> pieces;
    for (auto& it : positions)
    {
        pieces.push_back(Piece(it.x, it.y));
    }

    field->Dump(Dump);

    return 0;
}
