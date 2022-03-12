#include <iostream>
#include "Field.h"


int main()
{
    auto field = std::make_unique<Field>();
    field->Create(20, 20);

    auto f = [](const Field::CellType** cells, int width, int height)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                std::cout << static_cast<uint32_t>(cells[y][x]) << " ";
            }
            std::cout << std::endl;
        }
    };

    field->Dump(f);

    return 0;
}
