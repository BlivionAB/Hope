#include "FirstCharacterHash.h"

template<typename T>
std::size_t
FirstCharacterHash<T>::operator () (const Utf8Span& value, std::size_t tableSize) const
{
    return value[0] % tableSize;
}

template<typename T>
std::size_t
FirstCharacterHash<T>::operator () (const Utf8Span* value, std::size_t tableSize) const
{
    return (*value)[0] % tableSize;
}

template<typename T>
std::size_t
FirstCharacterHash<T>::operator () (const Utf8String& value, std::size_t tableSize) const
{
    return value[0] % tableSize;
}

template<typename T>
std::size_t
FirstCharacterHash<T>::operator () (const Utf8StringView& value, std::size_t tableSize) const
{
    return value[0] % tableSize;
}

template<typename T>
std::size_t
FirstCharacterHash<T>::operator () (const char* value, std::size_t tableSize) const
{
    return value[0] % tableSize;
}

template<typename T>
template<typename C, class>
std::size_t
FirstCharacterHash<T>::operator () (C value, std::size_t tableSize) const
{
    return static_cast<std::size_t>(value) % tableSize;
}
