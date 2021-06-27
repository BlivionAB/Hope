#ifndef FLASHPOINT_VECTOR_IMPL_H
#define FLASHPOINT_VECTOR_IMPL_H

#include "List.h"
#include "./Utf8String.h"
#include <iostream>


namespace elet::foundation
{


template<typename T>
List<T>::List():
    _capacity(1)
{
        auto s = sizeof(T);
    _items = reinterpret_cast<T*>(calloc(_capacity, sizeof(T)));
    _cursor = _items;
}


template<typename T>
List<T>::List(const List& other)
{
    _items = reinterpret_cast<T*>(calloc(other._capacity, sizeof(T)));
    _cursor = _items;
    for (const T& item : other)
    {
        *_cursor = item;
        _cursor++;
    }
    _cursor = _items + other.size();
    _capacity = other._capacity;
}


template<typename T>
List<T>::List(const std::initializer_list<T>& items):
    _capacity(items.size())
{
    _items = reinterpret_cast<T*>(calloc(items.size(), sizeof(T)));
    _cursor = _items;

    for (const T& item : items)
    {
        emplace(item);
    }
}


template<typename T>
List<T>::~List()
{
    for (T& item : *this)
    {
        item.~T();
    }
    free(_items);
}


template<typename T>
template<typename ... Args>
T*
List<T>::emplace(const Args&... args)
{
    static constexpr std::size_t sizeOfType = sizeof(T);
    std::size_t oldSize = size();
    if (oldSize + 1 > _capacity)
    {
        _capacity = _capacity * 2;
        _items = reinterpret_cast<T*>(realloc(_items, _capacity * sizeOfType));
        _cursor = _items + oldSize;
    }
    auto obj = new (_cursor) T(args...);
    _cursor++;
    return obj;
}


template<typename T>
void
List<T>::create(const T& item)
{
    static constexpr std::size_t sizeOfType = sizeof(T);
    std::size_t oldSize = size();
    if (oldSize + 1 > _capacity)
    {
        _capacity = _capacity * 2;
        T* oldItems =_items;
        _items = reinterpret_cast<T*>(calloc(_capacity, sizeOfType));
        std::memcpy(_items, oldItems, oldSize * sizeOfType);
        free(oldItems);
        _cursor = _items + oldSize;
    }
    *_cursor = item;
    _cursor++;
}


template<typename T>
void
List<T>::create(const List<T>& items)
{
    for (const T& item : items)
    {
        emplace(item);
    }
}


template<typename T>
void
List<T>::add(const T& item)
{
    std::size_t sizeOfType = sizeof(T);
    std::size_t oldSize = size();
    if (oldSize + 1 > _capacity)
    {
        _capacity = _capacity * 2;
        T* oldItems =_items;
        _items = reinterpret_cast<T*>(calloc(_capacity, sizeOfType));
        std::memcpy(_items, oldItems, oldSize * sizeOfType);
        free(oldItems);
        _cursor = _items + oldSize;
    }
    *_cursor = item;
    _cursor++;
}


template<typename T>
void
List<T>::add(const List<T>& items)
{
    for (const T& item : items)
    {
        add(item);
    }
}


template<typename T>
void
List<T>::add(const T&& item)
{
    std::size_t sizeOfType = sizeof(T);
    std::size_t oldSize = size();
    if (oldSize + 1 > _capacity)
    {
        _capacity = _capacity * 2;
        T* oldItems =_items;
        _items = reinterpret_cast<T*>(calloc(_capacity, sizeOfType));
        std::memcpy(_items, oldItems, oldSize * sizeOfType);
        free(oldItems);
        _cursor = _items + oldSize;
    }
    *_cursor = item;
    _cursor++;
}


template<typename T>
List<T>
List<T>::concat(const List<T>& other) const
{
    List<T> result;
    for (const T& item : *this)
    {
        result.emplace(item);
    }
    for (const T& item : other)
    {
        result.emplace(item);
    }
    return result;
}


template<typename T>
void
List<T>::remove(const T& item)
{
    std::size_t offset = 0;
    for (const T& storedItem : *this)
    {
        if (storedItem == item) {
            break;
        }
        ++offset;
    }
    if (offset == size() - 1)
    {
        _cursor--;
        return;
    }
    std::memcpy(_items + offset, _items + offset + 1, (_cursor - (offset + _items)));
    _cursor--;
}


template<typename T>
std::size_t
List<T>::size() const
{
    return _cursor - _items;
}


template<typename T>
void
List<T>::clear()
{
    _cursor = _items;
}


template<typename T>
T&
List<T>::last()
{
    return (*this)[this->lastIndex()];
}

template<typename T>
Utf8String
List<T>::join(Utf8String delimiter) const
{
    Utf8String result;
    std::size_t lastIndex = size() - 1;
    std::size_t index = 0;

    for (const Utf8String& item : *this)
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

template<typename T>
bool
List<T>::isEmpty() const
{
    return _cursor == _items;
}

template<typename T>
template<typename TType>
TType
List<T>::find(std::function<bool(T)> predicate, typename std::enable_if<std::is_pointer<TType>::value>::type*) const
{
    for (T& item : *this)
    {
        if (predicate(item))
        {
            return item;
        }
    }
    return nullptr;
}


template<typename T>
template<typename TType>
TType*
List<T>::find(std::function<bool(T&)> predicate, typename std::enable_if<!std::is_pointer<TType>::value>::type*) const
{
    for (T& item : *this)
    {
        if (predicate(item))
        {
            return &item;
        }
    }
    return nullptr;
}

template<typename T>
bool
List<T>::has(T item) const
{
    T& searchedItem = item;
    for (T& storedItem : *this)
    {
        if (storedItem == searchedItem)
        {
            return true;
        }
    }
    return false;
}


template<typename T>
template<typename TNew>
List<TNew>
List<T>::map(std::function<TNew (const T&)> mapper) const
{
    List<TNew> result;
    for (const T& item : *this)
    {
        TNew res = mapper(item);
        result.add(res);
    }
    return result;
}

template<typename T>
void
List<T>::swap(T *a, T *b)
{
    T t = *a;
    *a = *b;
    *b = t;
}

template<typename T>
T&
List<T>::operator [](std::size_t index) const
{
    return *(_items + index);
}


template<typename T>
List<T>&
List<T>::operator = (const List<T>& other)
{
    if (_items)
    {
        _items = reinterpret_cast<T*>(realloc(_items, other._capacity * sizeof(T)));
    }
    else
    {
        _items = reinterpret_cast<T*>(malloc(other._capacity * sizeof(T)));
    }
    _cursor = _items;
    for (const T& item : other)
    {
        new (_cursor) T(item);
        _cursor++;
    }
    _capacity = other._capacity;
    return *this;
}


template<typename T>
int
List<T>::partition(int low, int high, std::function<int(T &, T &)> comparator)
{
    T& pivot = (*this)[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (comparator((*this)[j], pivot) <= 0)
        {
            i++;
            swap(&(*this)[i], &(*this)[j]);
        }
    }
    swap(&(*this)[i + 1], &(*this)[high]);
    return (i + 1);
}


template<typename T>
void
List<T>::sort(std::function<int (T &, T &)> comparator)
{
    quickSort(0, size() - 1, comparator);
}


template<typename T>
void
List<T>::quickSort(int low, int high, std::function<int (T &, T &)> comparator)
{
    if (low < high)
    {
        int pivot = partition(low, high, comparator);
        quickSort(low, pivot - 1, comparator);
        quickSort(pivot + 1, high, comparator);
    }
}

template<typename T>
typename List<T>::Iterator
List<T>::begin() const
{
    return Iterator(_items);
}

template<typename T>
typename List<T>::Iterator
List<T>::end() const
{
    return Iterator(_cursor);
}

template<typename T>
void
List<T>::reserve(std::size_t capacity)
{
    auto s = size();
    _items = reinterpret_cast<T*>(realloc(_items, capacity * sizeof(T)));
    if (!_items)
    {
        throw std::runtime_error("Reservation failed.");
    }
    _cursor = _items + s;
    std::memset(_cursor, 0, capacity - size());
    this->_capacity = capacity;
}


template<typename T>
std::size_t
List<T>::lastIndex()
{
    return size() - 1;
}


template<typename T>
T*
List<T>::cursor()
{
    return _cursor;
}


template<typename T>
template<typename B>
B*
List<T>::write(B* batch)
{
    if (size() + sizeof(B) > _capacity)
    {
        reserve(size() + sizeof(B));
    }
    auto startCursor = reinterpret_cast<B*>(_cursor);
    std::memcpy(_cursor, batch, sizeof(B));
    _cursor += sizeof(B) / sizeof(T);
    return startCursor;
}


template<typename T>
List<T>
List<T>::slice(size_t index, size_t size)
{
    assert("size + index cannot be larger than size" && size + index <= this->size());

    List<T> list;
    list._items = reinterpret_cast<T*>(calloc(size, sizeof(T)));
    list._cursor = list._items;
    std::memcpy(list._items, _items + index, size * sizeof(T));
    list._cursor = list._items + size;
    list._capacity = size;
    return list;
}


template<typename T>
List <T>
List<T>::copy()
{
    return List<T>(*this);
}

template<typename T>
template<typename... Args>
void
List<T>::unshiftEmplace(const Args& ... args)
{
    std::size_t oldSize = size();
    if (oldSize + 1 > _capacity)
    {
        _capacity = _capacity * 2;
        _items = reinterpret_cast<T*>(realloc(_items, _capacity * sizeof(T)));
        _cursor = _items + oldSize;
    }
    new (_cursor) T(args...);
    _cursor++;
}



}


#include "Utf8String.h"

#endif // FLASHPOINT_VECTOR_IMPL_H