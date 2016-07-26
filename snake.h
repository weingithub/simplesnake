#ifndef _SIMPLE_SNAKE_H
#define _SIMPLE_SNAKE_H

#include "appdefine.h"
#include <list>

class CSimpleSnake
{
public:
    CSimpleSnake();
    ~CSimpleSnake();

    enum
    {
        DIRECTION_RIGHT = 0X1,
        DIRECTION_LEFT,
        DIRECTION_UP,
        DIRECTION_DOWN,        
    };
    
    typedef struct stPosInfo
    {
        uint32_t x;
        uint32_t y;
    }TPos;
    
    
    static void * ListenKeybordThread(void * vpParam);
    
    int Init(uint32_t x, uint32_t y);

    int DrawBox(char ch);
    
    int DrawSnake();
    
    int Run();
    
    int Move(); 
    
    int CheckCollide();
    
    int EatFood();   //吃食物
     
    int PrintCalcuteInfo();
    
    int PrintImportInfo(const char * msg);   //打印重要的信息。比如升级、撞墙、自杀等信息
    
    int SetDiresction(uint8_t udir);
    
    int GameEnd();
    
private:
    int GetTerminalSize();    
    
    int ClearSnake();
    
    int SwapSnake();
    
    int CheckPosExist(uint32_t x, uint32_t y);

    int CreateFood();
    
    int ClearPrint(uint32_t x, uint32_t y, uint32_t len);
private:
    uint8_t  m_direction;
    uint32_t m_left;
    uint32_t m_right;
    uint32_t m_up;
    uint32_t m_down;
    
    //TPos    m_lastPos;
    TPos      m_foodPos;   //先假设只有一个食物
    TPos      m_lastPos;   //上次消失的节点
    
    uint32_t  m_speed;   //移动速度
    
    TPos      m_scorePos;  
    TPos      m_levelPos;
    TPos      m_NextPos;   //记录下次过关所需分数
    TPos      m_DebugPos;   //debug信息打印的位置
    
    uint32_t  m_score;
    uint32_t  m_level;
    uint32_t  m_exp;  //每个食物代表的分数
    
    bool    m_isAllowChange;
    std::list<TPos>  m_snakebody;
};

#endif //_SIMPLE_SNAKE_H

