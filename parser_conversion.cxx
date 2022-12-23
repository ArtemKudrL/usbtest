#include "parser_conversion.h"

#include <stdexcept>

// TODO mb use sstream
void fromHex(char* dest, const char* src)
{
    char buf[3] = {0, 0, 0};
    while (*src)
    {
        buf[0] = *src++;
        if (!*src)
            throw std::invalid_argument("Wrong data format");
        buf[1] = *src++;
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
