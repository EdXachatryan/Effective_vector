#ifndef EFF_VECTOR_H
#define EFF_VECTOR_H

#include <list>
#include <vector>
#include <initializer_list>
#include <assert.h>
#include <memory>
#include <algorithm>

//========================================================================================================================
//        
//      General usings and helper details
//              
//========================================================================================================================

#include <set>
#include <map>
using std::vector;
using std::list;
using std::unique_ptr;

#define Eff_Vector_Teplate_description  template<typename T, bool sorted, typename comparator>
#define Eff_Vector_Class_description Effective_Vector <T, sorted, comparator>

//========================================================================================================================
//        
//      Definition of class Effective Vector         
//              
//========================================================================================================================

template<typename T, bool sorted = false, typename comparator = std::less<T>>
class Effective_Vector
{
public:
    class Iterator;

    using Type = T;
    using iterator = Iterator;

    using reference = T&;
    using const_reference = const reference;

    using chunk = vector<Type>;
    using TableChunk = vector<chunk>;

public:
    explicit Effective_Vector(const size_t chunkSize = 0) : m_ChunkSize(chunkSize), m_CurrentChunk(0)
    {
        if (chunkSize)
            m_TableChunks.push_back(chunk(chunkSize));
        if (!m_ChunkSize)
            m_ChunkSize = 2;
    }

    Effective_Vector(std::initializer_list<Type> init_list)
    {
        m_TableChunks.push_back(chunk(init_list));
        m_ChunkSize = init_list.size();
    }

    ~Effective_Vector()
    {
        clear();
    }

    void clear()
    {
        m_TableChunks.clear();
    }

    void push_back(const Type&);
    void pop_back();

    Type& operator[](int pos)
    {
        return getValue(pos);
    }

    const Type& operator[](int pos) const
    {
        return getValue(pos);
    }

    reference at(size_t pos);
    const_reference at(size_t pos) const;

    reference front() { return m_TableChunks[0][0]; }
    const_reference front() const { return m_TableChunks[0][0]; }

    reference back() { return m_TableChunks[m_CurrentChunk][m_TableChunks[m_CurrentChunk].size() - 1]; }
    const_reference back() const { return  m_TableChunks[m_CurrentChunk][m_TableChunks[m_CurrentChunk].size() - 1]; }

    iterator begin()
    {
         if (m_TableChunks.empty())
             return iterator(nullptr, &m_TableChunks, 0);
         else
             return iterator(&m_TableChunks[0][0], &m_TableChunks, 0);
    }

    iterator end()
    {
        if (m_TableChunks.empty())
            return iterator(nullptr, &m_TableChunks, m_CurrentChunk);
        else
            return iterator(&((m_TableChunks[m_TableChunks.size() - 1])[m_TableChunks[m_CurrentChunk].size() - 1]) + 1, &m_TableChunks, m_CurrentChunk);
    }
   
    size_t capacity() const;
    size_t size() const;
    bool empty() const;

private:
    // helper functions

    reference getValue(size_t pos)
    {
        int chunkPos = pos / (m_ChunkSize) ? m_ChunkSize : 1;
        int valuePos = pos - (chunkPos * m_ChunkSize);
        return m_TableChunks[chunkPos][valuePos];
    }

private:
    size_t m_ChunkSize;
    size_t m_CurrentChunk;
    TableChunk m_TableChunks;
};

//========================================================================================================================
//        
//      Implementation of class Effective Vector
//              
//========================================================================================================================

Eff_Vector_Teplate_description
inline void Eff_Vector_Class_description::push_back(const Type& value)
{
        if (m_TableChunks.empty())
        {
            m_TableChunks.push_back(chunk());
            m_CurrentChunk = 0;
            m_TableChunks[m_CurrentChunk].reserve(m_ChunkSize);
        }
        else if (m_TableChunks[m_CurrentChunk].size() == m_ChunkSize)
        {
            m_TableChunks.push_back(chunk());
            ++m_CurrentChunk;
            m_TableChunks[m_CurrentChunk].reserve(m_ChunkSize);
        }

        m_TableChunks[m_CurrentChunk].push_back(value);
}

Eff_Vector_Teplate_description
inline void Eff_Vector_Class_description::pop_back()
{
    assert(!this->empty(), "Empty Vector");

    if (m_TableChunks.at(m_CurrentChunk).empty() && m_CurrentChunk != 0)
    {
        m_TableChunks.at(m_CurrentChunk).shrink_to_fit();
        --m_CurrentChunk;
    }
    m_TableChunks.at(m_CurrentChunk).pop_back();

    if (m_TableChunks.at(m_CurrentChunk).empty())
    {
        --m_CurrentChunk;
    }
}


Eff_Vector_Teplate_description
inline typename Eff_Vector_Class_description::reference Eff_Vector_Class_description::at(size_t pos)
{
    if (pos >= size())
        throw std::out_of_range("pos >= size");

    return getValue(pos);
}


Eff_Vector_Teplate_description
inline typename Eff_Vector_Class_description::const_reference Eff_Vector_Class_description::at(size_t pos) const
{
    if (pos >= size())
        throw std::out_of_range("pos >= size");

    return getValue(pos);
}

Eff_Vector_Teplate_description
inline size_t Eff_Vector_Class_description::size() const
{
    return (!m_TableChunks.empty()) ? ((m_CurrentChunk * m_ChunkSize) + m_TableChunks[m_CurrentChunk].size()) : 0;
}

Eff_Vector_Teplate_description
inline bool Eff_Vector_Class_description::empty() const
{
    return !m_TableChunks.size();
}

Eff_Vector_Teplate_description
inline size_t Eff_Vector_Class_description::capacity() const
{
    return m_TableChunks.size() * m_ChunkSize;
}
//=========================== End of implementation of class Effective Vector ===============================================


//========================================================================================================================
//        
//      Implementation of  nested class Effective_Vector::Iterator            
//              
//========================================================================================================================

Eff_Vector_Teplate_description
class Eff_Vector_Class_description::Iterator
{
public:

    using pointer = Type*;
    using reference = Type&;

public:
    Iterator(Type* pos, TableChunk* table, const size_t currentChunk) : ref_Table(table)
    {
        m_ptr = pos;
        m_currentChunk = currentChunk;
    }

    Iterator& operator++()
    {
        if (m_ptr == &(ref_Table->at(m_currentChunk)).back() && ref_Table->size() > m_currentChunk + 1)
        {
            ++m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).at(0);
        }
        else
            ++m_ptr;

        return *this;
    }

    Iterator operator++(int) 
    {
        Iterator temp(*this);
        if (m_ptr != &(ref_Table->at(m_currentChunk)).back())
            ++m_ptr;
        else if (ref_Table->size() > m_currentChunk + 1)
        {
            ++m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).at(0);
        }
        return temp;
    }

    Iterator operator--(int)
    {
        Iterator temp(*this);
        if (m_ptr != &(ref_Table->at(m_currentChunk)).front())
            --m_ptr;
        else if (ref_Table->size() > m_currentChunk - 1)
        {
            --m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).back();
        }
        return temp;
    }

    Iterator& operator--()
    {
        if (m_ptr != &(ref_Table->at(m_currentChunk)).front())
            --m_ptr;
        else if (ref_Table->size() > m_currentChunk - 1)
        {
            --m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).back();
        }
        return *this;
    }
    const reference operator* () const { return *m_ptr; }
    const pointer operator->() const { return m_ptr; }

    reference operator* () { return *m_ptr; }
    pointer operator->() { return m_ptr; }

  

    friend bool operator==(const Iterator& left, const Iterator& right)
    {
        return (left.m_ptr == right.m_ptr);
    }

    friend bool operator!=(const Iterator& left, const Iterator& right)
    {
        return (left.m_ptr != right.m_ptr);
    }

private:
    Type* m_ptr;
    TableChunk* ref_Table;
    int m_currentChunk;
};

//=========================== End of Implementation of class Effective Vector Effective_Vector::Iterator =====================================

#endif // !EFF_VECTOR_H