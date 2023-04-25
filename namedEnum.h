#pragma once
#include <algorithm>
#include <optional>
#include <iterator>

template<typename EnumType, typename ValueType, int Count>
class NamedEnum {
public:   

    struct Item {
        EnumType key;
        ValueType value;        
    };

    using EnumUnderlyingType = std::conditional_t<std::is_enum_v<EnumType>,std::underlying_type_t<EnumType>, EnumType>;
    constexpr static bool is_incrementable = std::incrementable<EnumType>;
    constexpr static bool is_ordered = requires{ std::declval<ValueType>() <  std::declval<ValueType>();};
    union ItemStorage {
        Item x;
        constexpr ItemStorage() {}
        constexpr ~ItemStorage() {}
        constexpr const Item *operator->() const {return &x;};
    };

    constexpr static int count = Count;

    struct ValueIndexArray {
        int pos[Count] = {};
    };

    struct ValueIndexNone {};

    using ValueIndex = std::conditional_t<is_ordered,ValueIndexArray,ValueIndexNone>;

    constexpr NamedEnum(const Item (&items)[Count]) {
        int order[Count];
        for (int i = 0; i < Count; i++) {
            order[i] = i;
        }
        std::sort(std::begin(order), std::end(order), [&](int a, int b) {
            const Item &ia = items[a];
            const Item &ib = items[b];
            if (ia.key == ib.key) return a<b;
            else return ia.key < ib.key;            
        });        
        for (int i = 0; i < Count; i++) {
            std::construct_at(&_items[i].x, items[order[i]]);
        }
        initIndex();
    }

    constexpr bool is_sequence() const {
        if constexpr(is_incrementable || std::is_enum_v<EnumType>) {
            EnumType itr = _items[0]->key;
            for (int i = 1; i < Count; i++) {
                if constexpr(std::is_enum_v<EnumType>) {
                    itr = static_cast<EnumType>(static_cast<EnumUnderlyingType>(itr)+1);
                } else {
                    ++itr;
                }
                if (_items[i]->key != itr) return false;
            } 
            return true;
        } else {
            return false;
        }        
    }

    static constexpr ValueType defaultValue = {};
    static constexpr EnumType defaultEnum = {};

    constexpr const ValueType &get(const EnumType &evalue, const ValueType &defval = defaultValue) const {
        auto iter = find(evalue);
        if (iter == end()) return defval;
        else return iter->value;
    }

    constexpr const EnumType &get(const ValueType &v, const EnumType &defval = defaultEnum) const {
        auto iter = find(v);
        if (iter == end()) return defval;
        else return iter->key;
    }

    constexpr const ValueType &operator[](const EnumType &evalue) const {return get(evalue);}
    constexpr const EnumType &operator[](const ValueType &v) const {return get(v);}
        

    static constexpr int size() {return Count;}

    class Iterator {
    public:
        typedef std::ptrdiff_t difference_type;
        typedef Item value_type;
        typedef const Item* pointer;
        typedef const Item& reference;
        typedef std::random_access_iterator_tag iterator_category;

        constexpr Iterator(const ItemStorage *ptr):_ptr(ptr) {}
        constexpr reference operator *() const {return _ptr->x;}
        constexpr pointer operator->() const {return &_ptr->x;}
        Iterator &operator++() {++_ptr; return *this;}
        Iterator &operator--() {--_ptr; return *this;}
        Iterator &operator+=(ptrdiff_t x) {_ptr+=x; return *this;}
        Iterator &operator-=(ptrdiff_t x) {_ptr-=x; return *this;}
        Iterator operator++(int) {auto me = *this; ++_ptr; return me;}
        Iterator operator--(int) {auto me = *this; --_ptr; return me;}
        Iterator operator+(ptrdiff_t x) {return Iterator(_ptr+x);}
        Iterator operator-(ptrdiff_t x) {return Iterator(_ptr-x);}
        bool operator==(const Iterator &other) const {return _ptr == other._ptr;}
    protected:
        const ItemStorage *_ptr;

    };

    constexpr Iterator begin() const {return Iterator(_items);}
    constexpr Iterator end() const {return Iterator(_items+Count);}
    constexpr auto rbegin() const {return std::make_reverse_iterator<Iterator>(end());}
    constexpr auto rend() const {return std::make_reverse_iterator<Iterator>(begin());}

    constexpr Iterator find(const EnumType &evalue) const {
        if (_sequence) {
            if (evalue >= _items[0]->key && evalue <= _items[Count-1]->key) {
                int offset = static_cast<int>(static_cast<EnumUnderlyingType>(evalue) - static_cast<EnumUnderlyingType>(_items[0]->key));
                return Iterator(_items+offset);
            }
            return end();
        } else {
            auto iter = std::lower_bound(std::begin(_items), std::end(_items), evalue, [] <typename A, typename B>(const A &a, const B &b){
                if constexpr(std::is_same_v<A, EnumType>) {
                    return a < b->key;
                } else {
                    return a->key < b;
                }
            });
            if (iter == std::end(_items) || (*iter)->key != evalue) return end();
            else return Iterator(iter);
        }
    }

    constexpr Iterator find(const ValueType &v) const {
        if constexpr(is_ordered) {
            auto iter = std::lower_bound(std::begin(_valueIndex.pos), std::end(_valueIndex.pos), v, [&]<typename A, typename B>(const A &a, const B &b){
                if constexpr(std::is_same_v<A, ValueType>) {
                    return a < _items[b]->value;
                } else {
                    return _items[a]->value < b;
                }
            });
            if (iter == std::end(_valueIndex.pos) || _items[*iter]->value != v) return end();
            else return Iterator(&_items[*iter]);
        } else {
            auto iter = std::find_if(std::begin(_items), std::end(_items), [&](const ItemStorage &x) {
                return x->value == v;
            });
            if (iter == std::end(_items) || (*iter)->value != v) return end();
            else return Iterator(iter);
        }
    }

protected:
    ItemStorage _items[Count] = {};
    bool _sequence;
    ValueIndex _valueIndex;

    NamedEnum() = default;

    constexpr void initIndex() {
        _sequence = is_sequence();
        if constexpr(is_ordered) {
            for (int i = 0; i < Count; i++) {
                _valueIndex.pos[i] = i;
            }
            std::sort(std::begin(_valueIndex.pos),std::end(_valueIndex.pos), [&](int a, int b){
                return _items[a]->value < _items[b]->value;
            });
        }
    }
    
};
template<typename EnumType, typename ValueType, int N>
inline constexpr auto makeNamedEnum(const typename NamedEnum<EnumType, ValueType, N>::Item (&x)[N]) {
    return NamedEnum<EnumType,ValueType, N>(x);
}

namespace _named_enum_details {

template<typename UnderlyingEnumType, typename Iter, typename Fn>
inline constexpr void enumSyntaxParser(Iter iter, Iter end, Fn fn) {
    char collect[256];
    int collect_pos = 0;
    UnderlyingEnumType idx = 0;
    std::optional<UnderlyingEnumType> newidx;
    enum State {ident,  number, decimal, octal, hex};
    State st = ident;
    while (iter != end) {
        if (*iter == ',') {
            if (collect_pos) {
                if (newidx.has_value()) idx = *newidx;
                fn(std::string_view(collect, collect_pos), idx);
                ++idx;
            }
            collect_pos =0;
            newidx.reset();
            st = ident;
            ++iter;
            continue;
        }

        switch (st) {
            case ident: if ((*iter>='0' && *iter <='9') || (*iter == '_') || (*iter >= 'a' && *iter <= 'z') || (*iter >= 'A' && *iter <= 'Z')) {
                            collect[collect_pos++] = *iter;
                        } else  if (*iter == '=') {
                            st = number;
                        }                
                ++iter;
                break;
            case number: if (*iter == '0') {
                            st = octal; 
                            newidx = 0;
                        } else if (*iter > '0' && *iter <= '9') {
                            st = decimal;
                            newidx = (*iter - '0');
                        }
                    
                ++iter;
                break;
            case decimal: if (*iter >= '0' && *iter <= '9') {
                            newidx = *newidx * 10 + (*iter - '0');
                          }
                ++iter;
                break;
            case octal: 
                    if (*iter>='0' && *iter < '8') {
                        newidx = *newidx * 8 + (*iter - '0');
                    } else if (*iter == 'x') {
                        st = hex;
                    }                
                ++iter;
                break;
            case hex: 
                    if (*iter>='0' && *iter <= '9') {
                        newidx = *newidx * 16 + (*iter - '0');
                    } else if (*iter>='a' && *iter <= 'f') {
                        newidx = *newidx * 16 + (*iter - 'a' + 10);
                    } else if (*iter>='A' && *iter <= 'F') {
                        newidx = *newidx * 16 + (*iter - 'A' + 10);
                    }                 
                ++iter;
                break;
        }
    }
    if (collect_pos) {
        if (newidx.has_value()) idx = *newidx;
        fn(std::string_view(collect, collect_pos), idx);
    }
}

template<typename EnumType, auto string_fn>
inline constexpr int enumCountItems = ([]() {
    int count = 0;
    std::string_view text = string_fn();
    enumSyntaxParser<std::underlying_type_t<EnumType> >(text.begin(), text.end(),[&](auto a, auto b){++count;});
    return count;
})();

}

template<typename EnumType, auto string_fn>
class StringNamedEnum: public NamedEnum<EnumType, std::string_view, _named_enum_details::enumCountItems<EnumType, string_fn> > {

public:
    using Super = NamedEnum<EnumType, std::string_view, _named_enum_details::enumCountItems<EnumType, string_fn> >;
    using EnumUnderlyingType = typename Super::EnumUnderlyingType;


    static constexpr int string_area_size = ([]{
        int count = 0;
        std::string_view text = string_fn();
        _named_enum_details::enumSyntaxParser<std::underlying_type_t<EnumType> >(text.begin(), text.end(),[&](auto a, auto b){ count += a.size()+1;});
        return count;
    })();

    constexpr StringNamedEnum() {        
        init_content();
    }

    constexpr const char *get_string_area() const {return _string_area;}

protected:
    char _string_area[string_area_size];

    constexpr void init_content() {
        int strpos = 0;
        int tblpos = 0;
        std::string_view text = string_fn();
        _named_enum_details::enumSyntaxParser<std::underlying_type_t<EnumType> >(text.begin(), text.end(),[&](std::string_view text, EnumUnderlyingType idx ){
            std::copy(text.begin(), text.end(), _string_area+strpos);
            std::string_view tref(_string_area+strpos, text.size());
            strpos+=text.size();
            _string_area[strpos] = 0;
            ++strpos;

            std::construct_at(&Super::_items[tblpos].x, typename Super::Item{
                static_cast<EnumType>(idx), tref
            });
            ++tblpos;
        });
        std::sort(std::begin(Super::_items), std::end(Super::_items), [](const auto &a, const auto &b) {
            return a->key < b->key;
        });
        Super::initIndex();
    }

};

#define NAMED_ENUM(Typename, ...) enum class Typename { __VA_ARGS__}; \
using NamedEnum_##Typename =  StringNamedEnum<Typename, []{return #__VA_ARGS__;}>;
