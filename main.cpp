#include "namedEnum.h"
#include <iostream>
#include <string_view>

enum class Test {
    a,b,c=10,d,e
};

struct NonComparableStr {
    std::string_view s;
    template<int i>
    constexpr NonComparableStr(const char (&s)[i]):s(s,i) {}
    constexpr NonComparableStr() {}
    constexpr bool operator==(const NonComparableStr &o) const {return s == o.s;}
};



constexpr auto str = makeNamedEnum<Test,std::string_view>({
    {Test::a, "a"},
    {Test::b, "b"},
    {Test::c, "cc"},
    {Test::c, "c"},
    {Test::d, "d"},
    {Test::e, "e"}
});


consteval int count_comma(const std::string_view x) {
    int ret = 0;
    for (const char c: x) ret = ret  + (c == ',');
    return ret;
}

template<int> struct TestClass {};


consteval auto firstChar2(const std::string_view x) {
    constexpr int z = count_comma("aabbc,ewy");    
    return TestClass<z>();    
}


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


    std::cout <<  test2str[Test2::hex] << std::endl;
    std::cout <<  test2str[Test2::ef] << std::endl;
    std::cout <<  test2str[Test2::ab] << std::endl;
    std::cout <<  test2str[Test2::hex2] << std::endl;
    std::cout <<  test2str[Test2::zz] << std::endl;
    
    std::cout <<  barvy2str[Barvy::cervena] << std::endl;
    std::cout <<  barvy2str[Barvy::modra] << std::endl;
    std::cout <<  barvy2str[Barvy::zelena] << std::endl;
    std::cout <<  barvy2str[Barvy::zluta] << std::endl;
}

