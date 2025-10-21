//
// Created by gorev on 21.10.2025.

#pragma once

#include <pch.h>

/**
 * @brief Free list vector contains deque of empty items in vector.
 * @details Vector can look like [10, 13, 5, 4, empty, 3, empty, 12].
 * Deque will contain indexes of all empty items - 4 and 6 in this case.
 * This container designed for Asset Manager to prevent vector defragmentation after assets being deleted.
 * @tparam T items class
 */
template <typename T> class FreeListVector
{
public:
    FreeListVector() = default;
    explicit FreeListVector(unsigned int count);
    FreeListVector(unsigned int count, const T &value);
    FreeListVector(std::initializer_list<T> init);

    template <typename... Args> size_t emplace(Args &&...args);
    size_t push_back(const T &value);
    size_t push_back(T &&value);
    void erase(size_t index);
    void erase(eastl::vector<T>::iterator it);

    bool is_occupied(size_t index);
    size_t next_free_index() const;
    size_t size() const;
    size_t actual_size() const;
    size_t capacity() const;
    bool empty() const;
    void clear();
    void reserve(size_t new_capacity);


    T &operator[](size_t pos);
    const T &operator[](size_t pos);
    T &at(size_t pos);
    const T &at(size_t pos);


    eastl::vector<T>::iterator begin();
    eastl::vector<T>::const_iterator begin() const;
    eastl::vector<T>::const_iterator cbegin() const;
    eastl::vector<T>::iterator end();
    eastl::vector<T>::const_iterator end() const;
    eastl::vector<T>::const_iterator cend() const;


    eastl::vector<T> &get_vector();
    const eastl::vector<T> &get_vector() const;

    size_t free_slots_count() const;
    bool has_free_slots() const;

private:
    eastl::vector<T> m_data;
    eastl::deque<unsigned int> m_freeList;
};


template <typename T>
FreeListVector<T>::FreeListVector(unsigned int count)
    : m_data(count)
{
}


template <typename T>
FreeListVector<T>::FreeListVector(unsigned int count, const T &value)
    : m_data(count, value)
{
}


template <typename T>
FreeListVector<T>::FreeListVector(std::initializer_list<T> init)
    : m_data(init)
{
}


template <typename T> template <typename... Args> size_t FreeListVector<T>::emplace(Args &&...args)
{
    if (!m_freeList.empty())
    {
        size_t freeIndex = m_freeList.front();
        m_freeList.pop();
        new (&m_data[freeIndex]) T(eastl::forward<Args>(args)...);
        return freeIndex;
    }

    m_data.emplace_back(eastl::forward<Args>(args)...);
    return m_data.size() - 1;
}


template <typename T> size_t FreeListVector<T>::push_back(const T &value)
{
    return emplace(value);
}


template <typename T> size_t FreeListVector<T>::push_back(T &&value)
{
    return emplace(eastl::move(value));
}


template <typename T> void FreeListVector<T>::erase(size_t index)
{
    if (index < m_data.size())
    {
        m_data[index].~T();
        m_freeList.push(index);
    }
}


template <typename T> void FreeListVector<T>::erase(typename eastl::vector<T>::iterator it)
{
    if (it >= m_vector.begin() && it < m_vector.end())
    {
        const size_t index = static_cast<size_t>(it - m_vector.begin());
        erase(index);
    }
}


template <typename T> bool FreeListVector<T>::is_occupied(size_t index)
{
    if (index >= m_data.size()) return false;

    return eastl::find(m_freeList.begin(), m_freeList.end(), index) == m_freeList.end();
}


template <typename T> size_t FreeListVector<T>::next_free_index() const
{
    if (!m_freeList.empty()) return m_freeList.front();

    return m_data.size();
}


/**
 * @return size of the vector (it can contain empty items)
 */
template <typename T> size_t FreeListVector<T>::size() const
{
    return m_data.size();
}


/**
 * @return actual size with no empty items
 */
template <typename T> size_t FreeListVector<T>::actual_size() const
{
    return m_data.size() - m_freeList.size();
}


template <typename T> size_t FreeListVector<T>::capacity() const
{
    return m_data.capacity();
}


template <typename T> bool FreeListVector<T>::empty() const
{
    return m_data.empty();
}


template <typename T> void FreeListVector<T>::clear()
{
    for (size_t i = 0; i < m_data.size(); ++i)
        if (is_occupied(i)) m_data[i].~T();

    m_data.clear();
    m_freeList.clear();
}


template <typename T> void FreeListVector<T>::reserve(size_t new_capacity)
{
    m_data.reserve(new_capacity);
}


template <typename T> T &FreeListVector<T>::operator[](size_t pos)
{
    return m_data[pos];
}


template <typename T> T &FreeListVector<T>::operator[](size_t pos)
{
    return m_data[pos];
}


template <typename T> T &FreeListVector<T>::at(size_t pos)
{
    return m_data[pos];
}


template <typename T> const T &FreeListVector<T>::at(size_t pos)
{
    return m_data[pos];
}


template <typename T> typename eastl::vector<T>::iterator FreeListVector<T>::begin()
{
    return m_data.begin();
}


template <typename T> typename eastl::vector<T>::const_iterator FreeListVector<T>::begin() const
{
    return m_data.begin();
}


template <typename T> typename eastl::vector<T>::const_iterator FreeListVector<T>::cbegin() const
{
    return m_data.cbegin();
}


template <typename T> typename eastl::vector<T>::iterator FreeListVector<T>::end()
{
    return m_data.end();
}


template <typename T> typename eastl::vector<T>::const_iterator FreeListVector<T>::end() const
{
    return m_data.end();
}


template <typename T> typename eastl::vector<T>::const_iterator FreeListVector<T>::cend() const
{
    return m_data.cend();
}


template <typename T> eastl::vector<T> &FreeListVector<T>::get_vector()
{
    return m_data;
}


template <typename T> const eastl::vector<T> &FreeListVector<T>::get_vector() const
{
    return m_data;
}


/**
 * @return count of empty slots in vector
 */
template <typename T> size_t FreeListVector<T>::free_slots_count() const
{
    return m_freeList.size();
}


/**
 * @return true if there is empty item in vector, false otherwise
 */
template <typename T> bool FreeListVector<T>::has_free_slots() const
{
    return !m_freeList.empty();
}