#pragma once

#include <vector>
#include <cstddef>
#include <inttypes.h>

namespace Utils
{
    template<typename T>class Vector
    {
    public:
        Vector(size_t size = 0) :
            m_Data(size)
        {
        }

        Vector(size_t size, T s) :
            m_Data(size, s)
        {
        }

        size_t size() const
        {
            return m_Data.size();
        }

        void resize(size_t size)
        {
            m_Data.resize(size);
        }

        T &operator [](size_t s)
        {
            return m_Data[s];
        }

        void push_back(const T &s)
        {
            m_Data.push_back(s);
        }

        void remove(uint32_t id)
        {
            m_Data.erase(m_Data.begin() + id, m_Data.begin() + id + 1);
        }

        void clear(bool freeMemory = false)
        {
            m_Data.clear();
            if(freeMemory)
                m_Data.shrink_to_fit();
        }

        typedef T value_type;

        std::vector<T> m_Data;
    };
}

