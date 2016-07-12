#ifndef _APPDEFIND_H_
#define _APPDEFIND_H_

typedef  unsigned char  uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int   uint32_t;

#ifdef __x86_64__
    typedef  unsigned  long uint64_t;
#else
    typedef  unsigned  long long  uint64_t;
#endif

#define MEMCPY(pDstData, pSrcData, sLen)        \
{												\
    if (NULL == pSrcData)                       \
    {                                           \
        pDstData = new char[1];                 \
        memset(pDstData, 0, 1);                 \
    }                                           \
    else                                        \
    {                                           \
        pDstData = new char[sLen + 1];          \
        memset(pDstData, 0, sLen + 1);          \
        memcpy(pDstData, pSrcData, sLen);       \
    }											\
}		

#endif
