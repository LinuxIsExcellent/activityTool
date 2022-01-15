#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

class Packet
{
    template <typename T>
    void writeAtom(T data)
    {
        size_t dwMemSize = m_pMemoryEnd - m_pOffset;
        if (dwMemSize < sizeof(T))
        {
            setSize(m_pMemoryEnd - m_pMemory + sizeof(data));
        }

        *(T*)m_pOffset = data;
        m_pOffset += sizeof(data);

        if (m_pOffset > m_pDataEnd)
            m_pDataEnd = m_pOffset;
    }

    void writeBuf(const void* lpBuffer, size_t dwSize)
    {
        size_t dwMemSize = m_pMemoryEnd - m_pMemory;
        if (dwMemSize < dwSize)
        {
            setSize(m_pMemoryEnd - m_pMemory + dwSize);
        }

        memcpy(m_pOffset, lpBuffer, dwSize);
        m_pOffset += dwSize;

        if (m_pOffset > m_pDataEnd)
        {
            m_pDataEnd = m_pOffset;
        }

    }
    
    void setSize(size_t dwNewSize)
    {
        char* pOldMemory = m_pMemory;

        dwNewSize = (dwNewSize + MemoryAlignmentSize) & (~(MemoryAlignmentSize - 1));

        char* pMemory = (char*)malloc(dwNewSize);

        size_t dwOffset = m_pOffset - m_pMemory;
        size_t dwLength = m_pDataEnd - m_pMemory;
    }


    template <typename T>
    inline Packet& operator << (T val)
    {
        if (sizeof(T) < sizeof (long long))
            writeAtom<T>(val);
        else writeBuf(&val, sizeof(val));

        return *this;
    }

    Packet& operator << (const char* val)
    {
        writeBuf(val, strlen(val));
        writeAtom<char>(0);
        return *this;
    }


    Packet& operator << (char* val)
    {
        writeBuf(val, strlen(val));
        writeAtom<char>(0);
        return *this;
    }

private:
    char* m_pMemory;
    char* m_pMemoryEnd;
    char* m_pOffset;
    char* m_pDataEnd;
    
    static const size_t MemoryAlignmentSize = 256;  //数据包内存长度对齐边界
};

int main ()
{

    return 0;
}
