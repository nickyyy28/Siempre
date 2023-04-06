#include <base/Console.h>
#include <Logger/console_color.h>
#include <iostream>
#include <sstream>

namespace siem {

#define XX(_color, COLOR, base)    \
Console& Console::_color()   \
{                           \
    m_font_color = base + COLOR##_CODE;\
    return *this;           \
}

XX(blue, BLUE, FONT_COLOR_BASE)
XX(red, RED, FONT_COLOR_BASE)
XX(black, BLACK, FONT_COLOR_BASE)
XX(yellow, YELLOW, FONT_COLOR_BASE)
XX(green, GREEN, FONT_COLOR_BASE)
XX(purple, PURPLE, FONT_COLOR_BASE)
XX(cyan, CYAN, FONT_COLOR_BASE)
XX(white, WHITE, FONT_COLOR_BASE)

XX(background_blue, BLUE, BACKGROUND_COLOR_BASE)
XX(background_red, RED, BACKGROUND_COLOR_BASE)
XX(background_black, BLACK, BACKGROUND_COLOR_BASE)
XX(background_yellow, YELLOW, BACKGROUND_COLOR_BASE)
XX(background_green, GREEN, BACKGROUND_COLOR_BASE)
XX(background_purple, PURPLE, BACKGROUND_COLOR_BASE)
XX(background_cyan, CYAN, BACKGROUND_COLOR_BASE)
XX(background_white, WHITE, BACKGROUND_COLOR_BASE)

#undef XX

#define XX(index, style) \
Console& Console::style(bool enable)    \
{   \
    styles[index] = enable; \
    return *this;           \
}

XX(0, highlight)
XX(1, italic)
XX(2, underline)
XX(3, blink)
XX(4, reverse)
XX(5, hide)
XX(6, strikethrough)

#undef XX

Console::~Console()
{
    ss << CONSOLE_END;
    std::stringstream prefix;
    prefix << "\e[";
    if (m_background_color != -1) {
        prefix << m_background_color << ';';
    }

    if (m_font_color != -1) {
        prefix << m_font_color << ';';
    }

#define XX(index, STYLE)    \
    if (styles[index]) {    \
        prefix << STYLE##_CODE << ';'; \
    }

    XX(0, HIGHLIGHT);
    XX(1, ITALIC);
    XX(2, UNDERLINE);
    XX(3, BLINK);
    XX(4, REVERSE);
    XX(5, HIDE);
    XX(6, STRIKETHROUGH)

#undef XX

    prefix << "6m";

    std::cout << prefix.str() << ss.str() << CONSOLE_END;
}

std::stringstream& Console::operator<<(const std::string &str)
{
    ss << str;
    return ss;
}

}