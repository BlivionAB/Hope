#ifndef FLASHPOINT_LIST_H
#define FLASHPOINT_LIST_H

#include "Foundation/Memory/MemoryPool.h"
#include "Utf8StringView.h"
#include <functional>
#include <optional>
//#include <valgrind/memcheck.h>

namespace elet::foundation {

template<typename T>
class List
{
public:
    struct Result
    {
        bool
        Success;

        T*
        Value;
    };

    class Iterator {
        friend class List;
    public:
        T&
        operator *() const
        {
            return *(value);
        }

        const Iterator&
        operator ++ ()
        {
            value++;
            return *this;
        }

        bool
        operator == (const Iterator &other) const
        {
            return value == other.value;
        }

        bool
        operator != (const Iterator &other) const
        {
            return value != other.value;
        }

        Iterator(T* value):
            value(value)
        { }

    private:

        T*
        value;
    };


    List(const std::initializer_list<T>& items);

    List();

    List(const List& other);

    ~List();

    template<typename ... Args>
    void
    emplace(const Args& ... args);

    void
    create(const T& item);

    void
    create(const List<T>& item);

    void
    add(const T& item);

    void
    add(const List<T>& item);

    void
    add(const T&& item);

    void
    remove(const T& item);

    List<T>
    concat(const List<T>& other) const;

    T&
    operator [] (std::size_t index) const;

    List<T>&
    operator = (const List<T>& vector);

    std::size_t
    size() const;

    void
    clear();

    T&
    last();

    Utf8String
    join(const Utf8StringView& delimiter) const;

    bool
    isEmpty() const;

    void
    reserve(std::size_t capacity);

    void
    sort(std::function<int(T &, T &)> comparator);

    template<typename TType = T>
    TType
    find(std::function<bool(T)> predicate, typename std::enable_if<std::is_pointer<TType>::value>::type* = 0) const;

    template<typename TType = T>
    TType*
    find(std::function<bool(T&)> predicate, typename std::enable_if<!std::is_pointer<TType>::value>::type* = 0) const;

    Iterator
    begin() const;

    Iterator
    end() const;

    bool
    has(T key) const;

    template<typename TNew>
    List<TNew>
    map(std::function<TNew (const T& item)> mapper) const;

private:

    std::size_t
    _capacity;

    T*
    _items;

    T*
    _cursor;

    void
    swap(T *a, T *b);

    int
    partition(int low, int high, std::function<int(T &, T &)> comparator);

    void
    quickSort(int low, int high, std::function<int(T &, T &)> comparator);

    std::size_t
    lastIndex();
};

}

#include "ListImpl.h"


#endif //FLASHPOINT_LIST_H
