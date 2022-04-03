#pragma once
#include <string>

namespace game
{

class Utility
{
public:
    static const std::string& EncodeRunLength(const std::string& src, std::string& dst);
    static const std::string& DecodeRunLength(const std::string& src, std::string& dst);
};

}