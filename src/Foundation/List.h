#ifndef FLASHPOINT_LIST_H
#define FLASHPOINT_LIST_H

#include "Foundation/Memory/MemoryPool.h"
#include "Utf8String.h"
#include <functional>
#include <optional>

namespace elet::foundation
{
    template<typename T>
    class List;

    template<typename T, typename TListItem>
    struct ContainerPtr
    {
        size_t
        index;

        List<TListItem>*
        list;

        T*
        operator ->() const
        {
            return reinterpret_cast<T*>(&(*list)[index]);
        }


        T&
        operator *() const
        {
            auto s = &(*list)[index];
            return *(reinterpret_cast<T*>(&(*list)[index]));
        }


        ContainerPtr():
            index(0),
            list(nullptr)
        {

        }


        ContainerPtr(const ContainerPtr& other):
            index(other.index),
            list(other.list)
        {

        }


        ContainerPtr(size_t index, List<TListItem>* list):
            index(index),
            list(list)
        {

        }
    };


    template<typename T>
    class List
    {
    public:

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

            const Iterator&
            peek() const
            {
                return Iterator(value + 1);
            }

            const Iterator&
            next()
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
        T*
        emplace(const Args& ... args);

        template<typename ... Args>
        void
        unshiftEmplace(const Args& ... args);

        void
        create(const T& item);

        void
        create(const List<T>& item);

        T*
        cursor();

        void
        setCursor(T* cursor);

        template<typename B>
        ContainerPtr<B, T>
        write(B* batch, size_t size);

        template<typename B>
        ContainerPtr<B, T>
        write(B* batch);

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

        List<T>
        slice(size_t index, size_t size);

        List<T>
        copy();

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
        join(Utf8String delimiter) const;

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
