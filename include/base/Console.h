#pragma once

#include <sstream>
#include <string>
#include <sys/cdefs.h>
#include <vector>
namespace siem {

class Console;

class Console{
public:
    Console() = default;
    ~Console();

    std::stringstream& operator<<(const std::string& str);

    Console& blue();
    Console& red();
    Console& yellow();
    Console& black();
    Console& green();
    Console& purple();
    Console& cyan();
    Console& white();

    Console& background_blue();
    Console& background_red();
    Console& background_yellow();
    Console& background_black();
    Console& background_green();
    Console& background_purple();
    Console& background_cyan();
    Console& background_white();

    Console& highlight(bool enable = true);
    Console& underline(bool enable = true);
    Console& blink(bool enable = true);
    Console& reverse(bool enable = true);
    Console& hide(bool enable = true);
    Console& strikethrough(bool enable = true);
    Console& italic(bool enable = true);

    Console& text()
    {
        return *this;
    }
private:
    int m_font_color = -1;
    int m_background_color = -1;
    std::stringstream ss;
    bool styles[7] = {0};
};

}