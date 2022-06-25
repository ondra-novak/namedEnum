#include <iostream>
#include "namedEnum.h"


NAMED_ENUM(Colors,
        red=10,
        green,
        blue = 5,
        black = 0x1A,
        magenta=0x2b,
        white = 0777,
        purple,
        brown,
        yellow
        );


static NamedEnum_Colors colors;
static NamedEnum_Colors colors_whole("Colors::","");
static NamedEnum_Colors colors_light("light_","");
static NamedEnum_Colors colors_suffix("","_sufx");


int main(int argc, char **argv) {

    std::cout << "green = " << colors[Colors::green] << std::endl;
    std::cout << "white = " << colors[Colors::white] << std::endl;
    std::cout << "Colors::red= " << colors_whole[Colors::green] << std::endl;
    std::cout << "Colors::magenta = " << colors_whole[Colors::magenta] << std::endl;
    std::cout << "light_blue = " << colors_light[Colors::blue] << std::endl;
    std::cout << "light_purple = " << colors_light[Colors::blue] << std::endl;
    std::cout << "yellow_sufx = " << colors_suffix[Colors::blue] << std::endl;
    std::cout << "brown_sufx = " << colors_suffix[Colors::blue] << std::endl;


    std::cout << "26 = " << static_cast<int>(colors_light["light_black"]) << std::endl;
    std::cout << "511 = " << static_cast<int>(colors["white"]) << std::endl;
    std::cout << "10 = " << static_cast<int>(colors_whole["Colors::red"]) << std::endl;
    std::cout << "11 = " << static_cast<int>(colors_suffix["green_sufx"]) << std::endl;
    try {
        std::cout << "exception = " << static_cast<int>(colors["test"]) << std::endl;
    } catch (const UnknownEnumException &e) {
        std::cout << "exception " <<  e.what() << std::endl;
    }



}
