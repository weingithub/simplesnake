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


// 清除屏幕 
#define CLEAR() printf("\033[2J") 

// 上移光标 
#define MOVEUP(x) printf("\033[%dA", (x)) 

// 下移光标 
#define MOVEDOWN(x) printf("\033[%dB", (x)) 

// 左移光标 
#define MOVELEFT(y) printf("\033[%dD", (y)) 

// 右移光标 
#define MOVERIGHT(y) printf("\033[%dC",(y)) 

// 定位光标 
//先y坐标，再x坐标
#define MOVETO(x,y) printf("\033[%d;%dH", (y), (x)) 

// 光标复位 
#define RESET_CURSOR() printf("\033[H") 

// 隐藏光标 
#define HIDE_CURSOR() printf("\033[?25l") 

// 显示光标 
#define SHOW_CURSOR() printf("\033[?25h") 

#define CTRLC    3
#define SPACE    32
#define ESC      27
#define UP       65
#define DOWN     66
#define RIGHT    67
#define LEFT     68

#endif
