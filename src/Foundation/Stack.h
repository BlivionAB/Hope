#ifndef FLASHPOINT_STACK_H
#define FLASHPOINT_STACK_H

#include "Utf8String.h"
#include <variant>
#include <type_traits>
#include <initializer_list>

namespace elet::foundation
{

template<typename T> struct is_variant : std::false_type {};

template<typename ...Args>
struct is_variant<std::variant<Args...>> : std::true_type {};


template<typename T>
class Stack
{
public:

    Stack();

    Stack(const std::initializer_list<T>& chunks);

    explicit
    Stack(std::size_t size);

    Stack(const Stack& other);

    Stack(const Stack&& other) noexcept;

    ~Stack();

    void
    push(const T& item);

    void
    pop();

    template<typename ... Args>
    T&
    create(Args ... args);

    template<typename OneOfT, typename ... Args, typename TType = T, std::enable_if_t<is_variant<TType>::value, int> = 0>
    OneOfT&
    create(Args ... args);

    T&
    top() const;

    void
    clear();

    Utf8String
    join(const Utf8String& delimiter) const;

    void
    reserve(std::size_t capacity);

    bool
    isEmpty() const;

    std::size_t
    size() const;

    void
    reverse();

    class Iterator;

    Iterator
    begin() const;

    Iterator
    end() const;

    Stack&
    operator = (const Stack& other);

    T&
    operator [] (std::size_t index) const;

private:

    std::size_t
    _capacity;

    T*
    _items;

    T*
    _cursor;

public:
    class Iterator {
        friend class Stack;
    public:
        T&
        operator
        *() const
        {
            return *(value);
        }

        const Iterator&
        operator
        ++ ()
        {
            value++;
            return *this;
        }

        bool
        operator
        == (const Iterator &other) const
        {
            return value == other.value;
        }

        bool
        operator
        !=(const Iterator &other) const
        {
            return value != other.value;
        }

        explicit
        Iterator(T* value):
            value(value)
        { }

    private:

        T*
        value;
    };
};

}
#include "StackImpl.h"

#endif //FLASHPOINT_STACK_H
