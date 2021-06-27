#include "Stack.h"
#include <variant>

namespace elet::foundation
{

template<typename T>
Stack<T>::Stack():
    _capacity(1)
{
        auto s = sizeof(T);
    _items = reinterpret_cast<T*>(calloc(_capacity, sizeof(T)));
    _cursor = _items;
}

template<typename T>
Stack<T>::Stack(const std::initializer_list<T>& list):
    _capacity(list.size())
{
    _items = reinterpret_cast<T*>(calloc(_capacity, sizeof(T)));
    _cursor = _items;
    for (const T item : list)
    {
        push(item);
    }
}

template<typename T>
Stack<T>::Stack(std::size_t capacity):
    _capacity(capacity)
{
    _items = reinterpret_cast<T*>(calloc(capacity, sizeof(T)));
    _cursor = _items;
}

template<typename T>
Stack<T>::Stack(const Stack& other)
{
    _capacity = other._capacity;
    _items = reinterpret_cast<T*>(calloc(other._capacity, sizeof(T)));
    std::memcpy(_items, other._items, other.size() * sizeof(T));
    _cursor = _items + other.size();
}


template<typename T>
Stack<T>::Stack(const Stack&& other) noexcept
{
    _capacity = other._capacity;
    _items = reinterpret_cast<T*>(calloc(other._capacity, sizeof(T)));
    std::memcpy(_items, other._items, other.size() * sizeof(T));
}

template<typename T>
Stack<T>::~Stack()
{
    free(_items);
}

template<typename T>
void
Stack<T>::push(const T& item)
{
    if (size() == _capacity)
    {
        reserve(_capacity * 2);
    }
    *_cursor = item;
    _cursor++;
}

template<typename T>
template<typename ... Args>
T&
Stack<T>::create(Args ... args)
{
    if (size() == _capacity)
    {
        reserve(_capacity * 2);
    }
    _cursor = new (_cursor) T(args...);
    T* item = _cursor;
    _cursor++;
    return *item;
}

template<typename T>
template<typename OneOfT, typename ... Args, typename TType, std::enable_if_t<is_variant<TType>::value, int>>
OneOfT&
Stack<T>::create(Args ... args)
{
    if (size() == _capacity)
    {
        reserve(_capacity * 2);
    }
    _cursor = new (_cursor) T(OneOfT(args...));
    T* item = _cursor;
    _cursor++;
    return std::get<OneOfT>(*item);
}

template<typename T>
void
Stack<T>::clear()
{
    _cursor = _items;
}

template<typename T>
void
Stack<T>::pop()
{
    if (size() == 0)
    {
        return;
    }
    _cursor--;
}

template<typename T>
T&
Stack<T>::top() const
{
    return *(_cursor - 1);
}

template<typename T>
bool
Stack<T>::isEmpty() const
{
    return size() == 0;
}

template<typename T>
void
Stack<T>::reserve(std::size_t capacity)
{
    std::size_t oldSize = size();
    T* oldItems = _items;
    _items = reinterpret_cast<T*>(calloc(capacity, sizeof(T)));
    std::memcpy(_items, oldItems, oldSize * sizeof(T));
    free(oldItems);
    _cursor = _items + oldSize;
    _capacity = capacity;
}

template<typename T>
typename Stack<T>::Iterator
Stack<T>::begin() const
{
    return Iterator(_items);
}

template<typename T>
typename Stack<T>::Iterator
Stack<T>::end() const
{
    return Iterator(_cursor);
}

template<typename T>
Stack<T>&
Stack<T>::operator = (const Stack& other)
{
    if (_items != nullptr)
    {
        delete[] _items;
    }
    _items = reinterpret_cast<T*>(calloc(other._capacity, sizeof(T)));
    std::memcpy(_items, other._items, other.size() * sizeof(T));
    _capacity = other._capacity;
    _cursor = _items + other.size();
    return *this;
}

template<typename T>
std::size_t
Stack<T>::size() const
{
    return _cursor - _items;
}

template<typename T>
void
Stack<T>::reverse()
{
    Stack<T> reverseStack;
    std::size_t originalSize = size();
    reverseStack.reserve(size());
    while (!isEmpty())
    {
        reverseStack.push(top());
        pop();
    }
    std::memcpy(_items, reverseStack._items, originalSize * sizeof(T));
    _cursor = _items + originalSize;
}

template<typename T>
T&
Stack<T>::operator [] (std::size_t index) const
{
    return *(_items + index);
}

template<typename T>
Utf8String
Stack<T>::join(const Utf8String& delimiter) const
{
    Utf8String result;
    std::size_t __size = size();
    if (__size == 0)
    {
        return result;
    }
    std::size_t lastIndex = __size - 1;
    std::size_t index = 0;

    for (const T& item : *this)
    {
        if (index == lastIndex)
        {
            result += item;
        }
        else
        {
            result += item;
            result += delimiter;
        }
        index++;
    }
    return result;
}

}