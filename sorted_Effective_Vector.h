#ifndef SORTED_EFF_VECTOR_H
#define SORTED_EFF_VECTOR_H

#include "Effective_Vector_.h"
#define Sorted_Eff_Vector_Teplate_description  template<typename T, typename comparator>
#define Sorted_Eff_Vector_Class_description Effective_Vector <T, true, comparator>

//========================================================================================================================
//        
//      Specialization of class Effective Vector<T, sorted = true>         
//              
//========================================================================================================================

template <typename T, typename comparator>
class Effective_Vector<T, true, comparator>
{
public:
    class Iterator;
    using Type = T;
    using iterator = Iterator;
    using const_iterator = const iterator;

    using reference = T&;
    using const_reference = const reference;

    using chunk = vector<Type>;
    using orderChunks = std::pair<int, chunk>;
    using TableChunk = vector<orderChunks>;

public:
    explicit Effective_Vector(const size_t chunkSize = 0) : m_ChunkSize(chunkSize), m_CurrentChunk(0)
    {
        if (chunkSize)
            m_TableChunks.push_back({ 0, chunk(chunkSize) });
        m_elementSize = chunkSize;
        if (!m_ChunkSize)
            m_ChunkSize = 2;
    }

    Effective_Vector(std::initializer_list<Type> init_list)
    {
        m_TableChunks.push_back({ 0, chunk(init_list) });
        m_ChunkSize = init_list.size();
        m_elementSize = m_ChunkSize;
        std::sort(m_TableChunks[0].second.begin(), m_TableChunks[0].second.end(), compare());
    }

    ~Effective_Vector()
    {
        clear();
    }

    void std_sort()
    {
        chunk allChunks;
        allChunks.reserve(size());
        for (size_t i = 0; i < m_TableChunks.size(); i++)
        {
            chunk oneChunk = m_TableChunks.at(i).second;
            for (auto value : oneChunk)
            {
                allChunks.push_back(value);

            }
        }
        std::sort(allChunks.begin(), allChunks.end(), compare());
        m_TableChunks.clear();
        m_TableChunks.push_back(allChunks);
        m_CurrentChunk = 0;
    }

    void clear()
    {
        for (size_t i = 0; i < m_TableChunks.size(); ++i)
        {
            m_TableChunks.at(i).second.clear();
        }
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

    reference front() { return m_TableChunks[0].second[0]; }
    const_reference front() const { return m_TableChunks[0].second[0]; }

    reference back() { return m_TableChunks[m_CurrentChunk].second[m_TableChunks[m_CurrentChunk].second.size() - 1]; }
    const_reference back() const { return  m_TableChunks[m_CurrentChunk].second[m_TableChunks[m_CurrentChunk].second.size() - 1]; }

    iterator begin()
    {
        if (m_TableChunks.empty())
            return const_iterator{ nullptr, &m_TableChunks, 0 };
        else
            return const_iterator{ &(m_TableChunks[0]).second[0], &m_TableChunks, 0 };
    }

    iterator end()
    {
        if (m_TableChunks.empty())
            return iterator(nullptr, &m_TableChunks, m_CurrentChunk);
        else
            return iterator(&((m_TableChunks[m_TableChunks.size() - 1]).second[m_TableChunks[m_TableChunks.size() - 1].second.size() - 1]) + 1, &m_TableChunks, m_CurrentChunk);
    }
    size_t capacity() const;
    size_t size() const;
    bool empty() const;

private:
    // helper functions

    reference getValue(size_t pos)
    {
        int chunkPos = 0;
        int copyPos = pos;
        int valuePos = 0;

        while (true)
        {
            valuePos += m_TableChunks[chunkPos].second.size();
            if (valuePos == pos)
            {
                ++chunkPos;
                break;
            }
            if (valuePos < pos)
                ++chunkPos;
         
            else
                break;
        }

        int elemPos = -1;
        if (valuePos == pos)
            elemPos = 0;
        else
            elemPos = std::min(valuePos - pos, pos);
        return m_TableChunks[chunkPos].second[elemPos];
    }

    int getChunkIndex(const Type& value)
    {
        int n = (value / m_ChunkSize);
        return  (m_ChunkSize) ? (value / m_ChunkSize) : m_ChunkSize;
    }

    int binarySearch(int beg, int end, const Type& value)
    {
        if (end >= beg) {
            int mid = beg + (end - beg) / 2;

            if (m_TableChunks[mid].first == value)
                return mid;

            if (m_TableChunks[mid].first > value)
                return binarySearch(beg, mid - 1, value);

            return binarySearch(mid + 1, end, value);
        }
        return -1;
    }

    int binarySearchPosition(size_t beg, size_t end, int chunkNumber, const Type& value)
    {
        size_t begin = beg;
        size_t mid = beg + (end - beg) / 2;
        mid = std::max(mid, m_TableChunks[chunkNumber].second.size() - 1);
        if (m_TableChunks[chunkNumber].second[mid] == value)
            return mid;
        
        const chunk& refChunk = m_TableChunks[chunkNumber].second;
        if (m_TableChunks[chunkNumber].second[mid] > value)
        {
            while (++begin != --mid)
            {
                if (compare(refChunk[begin], value))
                {
                    return begin;
                }
                if (compare(refChunk[mid],value))
                    return mid;
            }
        }
        else
        {
            while (mid != --end)
            {
                mid = std::max(++mid, m_TableChunks[chunkNumber].second.size() - 1);
                if (compare(refChunk[mid], value))
                {
                    return mid;
                }
                if (compare(refChunk[end], value))
                    return end;
            }
        }
        return -1;
    }


private:
    size_t m_ChunkSize;
    size_t m_elementSize;
    size_t m_CurrentChunk;
    comparator compare;
    TableChunk m_TableChunks;
};

//========================================================================================================================
//        
//      Implementation of class  Effective_Vector template specialization for Sorted Effective Vector     
//              
//========================================================================================================================

Sorted_Eff_Vector_Teplate_description
inline void Sorted_Eff_Vector_Class_description::push_back(const Type& value)
{
    int chunkPriority = getChunkIndex(value);
    int chunkPos = binarySearch(0, m_TableChunks.size() - 1, chunkPriority);

    if (chunkPos != -1)
    {
        if (m_TableChunks[chunkPos].second.size() == m_ChunkSize)
        {
            chunk temp;
            temp.reserve(m_ChunkSize);
            temp.push_back(value);
            m_TableChunks.push_back({ chunkPriority, temp });
            ++m_elementSize;
            ++m_CurrentChunk;

            std::sort(m_TableChunks.begin(), m_TableChunks.end(), [](const orderChunks& left, const orderChunks& right)->bool {
                return left.first < right.first;
                }
            );
        }
        else
        {
            m_TableChunks[chunkPos].second.push_back(value);
            ++m_elementSize;
            std::sort(m_TableChunks[chunkPos].second.begin(), m_TableChunks[chunkPos].second.end(), comparator());
        }
        int samePriorityBEgin = chunkPos;

        for (auto it = m_TableChunks.begin() + chunkPos; it != m_TableChunks.begin(); --it)
        {
            if (it->first == chunkPriority)
            {
                --samePriorityBEgin;
            }
        }

        int insertPos = -1;
        for (; samePriorityBEgin != chunkPos; ++samePriorityBEgin)
        {
            insertPos = binarySearchPosition(0, m_TableChunks[samePriorityBEgin].second.size(), samePriorityBEgin, value);

            if (insertPos != -1)
            {
                Type& lastItem = m_TableChunks[samePriorityBEgin].second.back();
                m_TableChunks[samePriorityBEgin].second.insert(m_TableChunks[samePriorityBEgin].second.begin() + insertPos, value);
                ++samePriorityBEgin;
                while (++samePriorityBEgin != chunkPos)
                {
                    Type nextChunkLastItem = m_TableChunks[samePriorityBEgin].second.back();
                    m_TableChunks[samePriorityBEgin].second.insert(m_TableChunks[samePriorityBEgin].second.begin(), lastItem);
                    lastItem = nextChunkLastItem;
                }
            }
        }
    }
    else
    {
        chunk temp;
        temp.reserve(m_ChunkSize);
        temp.push_back(value);
        ++m_elementSize;
        m_TableChunks.push_back({ chunkPriority, temp });
        std::sort(m_TableChunks.begin(), m_TableChunks.end(), [](const orderChunks& left, const orderChunks& right)->bool {
            return left.first < right.first;
            }
        );
    }
    m_CurrentChunk = m_TableChunks.size() - 1;
}

Sorted_Eff_Vector_Teplate_description
inline void Sorted_Eff_Vector_Class_description::pop_back()
{
    assert(!this->empty(), "Empty Vector");

    if (m_TableChunks.at(m_CurrentChunk).second.empty() && m_CurrentChunk != 0)
    {
        m_TableChunks.at(m_CurrentChunk).second.shrink_to_fit();
        --m_CurrentChunk;
    }
    m_TableChunks.at(m_CurrentChunk).second.pop_back();
    if (m_TableChunks.at(m_CurrentChunk).second.empty())
    {
        m_TableChunks.erase(m_TableChunks.end() - 1);
        m_TableChunks.shrink_to_fit();
        --m_CurrentChunk;
    }
    --m_elementSize;
}

Sorted_Eff_Vector_Teplate_description
inline typename Sorted_Eff_Vector_Class_description::reference Sorted_Eff_Vector_Class_description::at(size_t pos)
{
    if (pos >= size())
        throw std::out_of_range("position >= size");

    return getValue(pos);
}

Sorted_Eff_Vector_Teplate_description
inline typename Sorted_Eff_Vector_Class_description::const_reference Sorted_Eff_Vector_Class_description::at(size_t pos) const
{
    if (pos >= size())
        throw std::out_of_range("position >= size");

    return getValue(pos);
}

Sorted_Eff_Vector_Teplate_description
inline size_t Sorted_Eff_Vector_Class_description::size() const 
{
    return m_elementSize;
}

Sorted_Eff_Vector_Teplate_description
inline bool Sorted_Eff_Vector_Class_description::empty() const
{
    return !m_TableChunks.size();
}

Sorted_Eff_Vector_Teplate_description
inline size_t Sorted_Eff_Vector_Class_description::capacity() const
{
    return m_TableChunks.size() * m_ChunkSize;
}

//=========================== End of implementation of specialized sorted Effective_Vector class ===============================================


//========================================================================================================================
//        
//      Implementation of nested class specialized Sorted Effective_Vector::Iterator            
//              
//========================================================================================================================
Sorted_Eff_Vector_Teplate_description
class Sorted_Eff_Vector_Class_description::Iterator
{
public:

    using pointer = Type*;
    using reference = Type&;
    using const_pointer = const pointer;
public:
    Iterator(Type* pos, TableChunk* table, const size_t currentChunk) : ref_Table(table)
    {
        m_ptr = pos;
        m_currentChunk = currentChunk;
    }

    Iterator& operator++()
    {
        if (m_ptr == &(ref_Table->at(m_currentChunk)).second.back() && ref_Table->size() > m_currentChunk + 1)
        {
            ++m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).second.at(0);
        }
        else
            ++m_ptr;

        return *this;
    }

    Iterator operator++(int)
    {
        Iterator temp(*this);
        if (m_ptr != &(ref_Table->at(m_currentChunk)).second.back())
            ++m_ptr;
        else if (ref_Table->size() > m_currentChunk + 1)
        {
            ++m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).second.at(0);
        }
        return temp;
    }

    Iterator operator--(int)
    {
        Iterator temp(*this);
        if (m_ptr != &(ref_Table->at(m_currentChunk)).second.front())
            --m_ptr;
        else if (ref_Table->size() > m_currentChunk - 1)
        {
            --m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).second.back();
        }
        return temp;
    }

    Iterator& operator--()
    {
        if (m_ptr != &(ref_Table->at(m_currentChunk)).second.front())
            --m_ptr;
        else if (ref_Table->size() > m_currentChunk - 1)
        {
            --m_currentChunk;
            m_ptr = &ref_Table->at(m_currentChunk).second.back();
        }
        return *this;
    }

    reference operator* () { return *m_ptr; }
    pointer operator->() { return m_ptr; }
    const_pointer operator->() const { return m_ptr; }

    Iterator& operator+=(size_t pos)
    {
        size_t row = pos / ref_Table->at(0).second.size();
        size_t col = pos - row * ref_Table->at(0).second.size();
        m_ptr = &ref_Table->at(row).second.at(col);
        return *this;
    }

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

//=========================== End of specialized sorted Effective_Vector::Iterator class implementation =====================================

#endif // !SORTED_EFF_VECTOR_H
