# NamedEnum 

Implements an C++ source interface to create enumertaion values (enums) which can 
be easily converted to strings and vice versa.


The library is distributed as a single header file, no additional link libraries are needed.

Required C++ version: c++11 is minimum, c++17 is recommended

## NamedEnum template

NamedEnum template `NamedEnum<Type>` is class which can be used to construct lookup
table between enumeration value and its string representation.

```
enum class Colors {red, green, blue, black, white};

NamedEnum<Colors> colors({
    {Colors::red,"red"},
    {Colors::green,"green"},
    {Colors::blue,"blue"},
    {Colors::black,"black"},    
    {Colors::white,"white"}
}
```

To convert enumeration value to a string, you just simply execute lookup `colors[value]` for example:

```
std::string blue_name = colors[Colors::blue]
```

It is also possible to perform reverse lookup from the string to the enumeration value

```
Colors c = colors["black"];
```

If the string value is not found in the lookup table, the UnknownEnumException is thrown. However, you can try to retrieve the enumeration value without throwing the exception:


With default value:

```
Colors c =  colors.get(s, Colors::black); // will return Color::black for unmatching text
```

Using nullptr to express unsuccessful lookup

```
const Colors *c = colors.find(s);
if (c == nullptr) {
    //not found
} else {
    //found
}
```

### Duplicated values

It is safe to put duplicated values to the table. It allows to assign for example
multiple names to a single enum value. However remember that if you request
reversed lookup, the function can choose randomly any row where the value
used as key for the lookup is duplicated. There is no way to specify preference.


### Enumration of items

You can enumerate all named items using range-for cycle.

For each item, you receive `.name` and `.value`

```
 for (const auto &item: colors) {
        std::cout << item.name << " = " << static_cast<int>(item.value) << std::endl;
    }
```


## Macro NAMED_ENUM

This macro simplifies process of creating the lookup table. Instead declaring the
`enum` and the table, you can use this macro to declare both

```
NAMED_ENUM(Colors, red, green, blue, black, white);
```

Result of this macro invocation is declaration of the `enum Colors` and also declaration of a class named `NamedEnum_Colors` which is already initialized with the lookup table. You
only need to create an instance. Its up to you, if you make the instance statically or
dynamically allocated

```
NamedEnum_Colors colors;
```

Rest of usage is similar as **NamedEnum template**

### Limitations

There are several limitations. The enumeration definition can have only symbol definition
itself and additionally an assigned value, but the value can only be numeric constant only in decimal, octal and hexadecimal base. Other representations are not supported. So you cannot use characters (char) or reference to other symbols. Expressions are not supported as well.

```
blue, // ok
green = 2,  // ok
black = 056, // ok
red = 0x1F, // ok
yellow = green + 2, //not supported
cian = const_a + const_b, //not supported
brown = 'b', //not supported  
```

If you need such features in your code, you need to create the lookup table manually (see NamedEnum template)

### Declare in class or namespace

There is no limitation on where the NAMED_ENUM can be declared. Note that generated class for the lookup table is also declared in the same namespace.

However, you can put the instance of the generated class to the different namespace. This can be useful in classes, if you need for example to make enum `public` but lookup table `protected`


### Prefix and suffix

You can also define a prefix and suffix for enumeration names

```
NamedEnum_Colors colors(prefix,suffix);
```

## TIP: multiple lookup tables for single 'enum'

Because the lookup table is not directly connected to the `enum` type itself, you can
have multiple lookup tables for single `enum` for various purposes.

