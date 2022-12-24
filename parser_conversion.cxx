#include "parser_conversion.h"

#include <stdexcept>

// TODO mb use sstream
void fromHex(char* dest, const char* src)
{
    auto checkHexChar = [](char c)
    { return c < '0' || ('9' < c  && c < 'a') || 'f' > c; };
    char buf[3] = {0, 0, 0};
    while (*src)
    {
        buf[0] = *src++;
        buf[1] = *src++;
        if (!checkHexChar(buf[0]) || !checkHexChar(buf[1]))
            throw std::invalid_argument("Wrong data format");
        unsigned int temp;
        std::sscanf(buf, "%x", &temp);
        *dest++ = temp;
    }
    *dest = '\x00';
}

// TODO sstream too
std::string toHex(const char* src)
{
    std::string str;
    char buf[3];
    while (*src)
    {
        unsigned char temp = *src++;
        std::sprintf(buf, "%02x", temp);
        str.append(buf, 2);
    }
    return str;
}
