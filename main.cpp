#include "namedEnum.h"
#include <iostream>
#include <string_view>

enum class Test {
    a,b,c=10,d,e
};


constexpr auto str = makeNamedEnum<Test,std::string_view>({
    {Test::a, "a"},
    {Test::b, "b"},
    {Test::c, "cc"},
    {Test::c, "c"},
    {Test::d, "d"},
    {Test::e, "e"}
});

NAMED_ENUM(Test2, 
            ab,
            cd,
            ef,
            xy=10,
            zz,
            ax =  011, 
            hex = 0xABAB,   
            hex2   
    );
 constexpr NamedEnum_Test2 test2str;

 NAMED_ENUM(Barvy, 
            cervena,
            modra,
            zluta,
            zelena
    );

constexpr NamedEnum_Barvy barvy2str;


int main() {    

    auto c = str[Test::c];
    std::cout << c << std::endl;

    bool r = (str["b"] == Test::b);
    std::cout << r << std::endl;

    std::cout << "----" <<std::endl;
    std::cout <<  test2str[Test2::hex] << std::endl;
    std::cout <<  test2str[Test2::ef] << std::endl;
    std::cout <<  test2str[Test2::ab] << std::endl;
    std::cout <<  test2str[Test2::hex2] << std::endl;
    std::cout <<  test2str[Test2::zz] << std::endl;
    std::cout << "----" <<std::endl;
    std::cout <<  barvy2str[Barvy::cervena] << std::endl;
    std::cout <<  barvy2str[Barvy::modra] << std::endl;
    std::cout <<  barvy2str[Barvy::zelena] << std::endl;
    std::cout <<  barvy2str[Barvy::zluta] << std::endl;
    std::cout << "----" <<std::endl;
    for (const auto &a: test2str) {
        std::cout << a.value << '=' << static_cast<int>(a.key) << std::endl;
    }
}

