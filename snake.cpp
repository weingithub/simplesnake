#include "snake.h"
#include "kbinit.h"
#include "common.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <pthread.h>

const int MAX_LEVEL = 10;

const int NeedScore[MAX_LEVEL] = {0, 100, 300, 600, 1000, 2000, 4000, 7000, 11000, 16000}; 
const int SpeedUp[MAX_LEVEL] = {0, 45, 37, 33, 30, 25, 20, 18, 15, 10}; 
const int DirectionCompare[4] = {0, 0, 1, 1};  //右左上下，判断是否在同一个方向。值相同，则方向相同

using namespace std;

CSimpleSnake::CSimpleSnake():
    m_direction(DIRECTION_RIGHT),
    m_left(1),
    m_right(1),
    m_up(1),
    m_down(1), 
    m_speed(1),
    m_score(0),
    m_level(1),
    m_exp(50),
    m_isAllowChange(true)
{
    
}

CSimpleSnake::~CSimpleSnake()
{
    
}

int CSimpleSnake::Init(uint32_t x, uint32_t y)
{
    //draw a x*y rectangle
    CLEAR();
        
    m_right = x;
    m_down = y;
   
   
    GetTerminalSize();  //可能会修改m_right和m_down的值

    DrawBox(' ');
    
    //设置分数和等级等的打印位置信息
    m_scorePos.x = m_right + 2;
    m_scorePos.y = m_down /2;    
    
    m_levelPos.x = m_scorePos.x;
    m_levelPos.y = m_scorePos.y + 1;  

    m_NextPos.x = m_scorePos.x;
    m_NextPos.y = m_scorePos.y + 2;
    
    m_DebugPos.x = m_scorePos.x;
    m_DebugPos.y = m_scorePos.y + 4;
    
    
    CreateFood();
    
    TPos snake;
    
    //初始化，只设置蛇头一个
    snake.x = m_left + 1;
    snake.y = m_up + 1;
    
    m_snakebody.push_back(snake);

    PrintCalcuteInfo();
    
    return 0;
}

int CSimpleSnake::DrawBox(char ch)
{
    //画一个游戏盒子

    uint8_t direction = DIRECTION_RIGHT;

    CLEAR();
    
    printf("\033[40;34m");
    
    while(true)
    {
        if (DIRECTION_RIGHT == direction)
        {
            MOVETO(m_left, m_up);
            
            for(uint32_t i = m_left; i <= m_right; ++i)
            {
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            direction = DIRECTION_DOWN;
        }
        else if (DIRECTION_DOWN == direction)
        {
            for(uint32_t j = m_up + 1; j < m_down; ++j)
            {
                MOVETO(m_right, j);
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            direction =  DIRECTION_LEFT;
        }
        else if(DIRECTION_LEFT == direction)
        {
            MOVETO(0, m_down);
            
            for(uint32_t i = m_left; i <= m_right; ++i)
            {
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            cout<<flush;
            
            direction = DIRECTION_UP;
        }
        else if (DIRECTION_UP == direction)
        {
            for(uint32_t j = m_up + 1; j < m_down; ++j)
            {
                MOVETO(0, j);
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            break;
        }
    }
    
    printf("\033[0m");  //关闭所有属性 
    //MOVETO(0, m_down+2);
}

int CSimpleSnake::Run()
{
    //隐藏光标
    HIDE_CURSOR();
    
    //让蛇跑起来
    int ret = 0;
    
    //设置定时信号
    //创建接收玩家键盘信息的子线程
    
    //创建线程
	pthread_t  threadT;
	pthread_attr_t attr;
				
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
    
    if (0 != pthread_create(&threadT, &attr, ListenKeybordThread, this))
	{
		cerr<<"创建线程失败"<<endl;
        return 1;
	}	
	else
	{
		//cout<<"创建线程成功"<<endl;
	}	
    
    //sleep(1);
    uint32_t times = 0;
    
    while(true)
    {
        times = SpeedUp[m_level];

        ret = Move();
        
        if (ret)
        {
            break;
        }
        
        //速度的提升，体现在刷新的间隔时间变少
        usleep(1000*10*times);
    }
    
    GameEnd();

    return 0;
}

int CSimpleSnake::Move()
{
    system("echo Move >> debug_log");
    //实现移动的效果，就是不断的重绘
    
    //先清除旧的位置，再打印新的位置

    ClearSnake();
    SwapSnake();
    
    //修改尾部指针的坐标即可
    
    list<TPos>::reverse_iterator riter = m_snakebody.rbegin();
    
    TPos & changepos = *riter;
    
    switch(m_direction)
    {
        case DIRECTION_RIGHT: changepos.x += m_speed; break;
        case DIRECTION_LEFT: changepos.x -= m_speed; break;
        case DIRECTION_UP: changepos.y -= m_speed; break;
        case DIRECTION_DOWN: changepos.y += m_speed; break;
    }

    int ret = CheckCollide();
    
    if (1 == ret)
    {
        DrawSnake();  //打印临时的场景
        return ret;
    }
    else if (2 == ret)   //吃到食物
    {
        int res = EatFood();   //是否达到最大级数
        
        if (res)
        {
            return res;
        }
        
    }
    
    DrawSnake();
    m_isAllowChange = true;
    
    return 0;
}

int CSimpleSnake::PrintCalcuteInfo()
{
    MOVETO(m_scorePos.x, m_scorePos.y);
    
    cout<<"分数："<<m_score<<flush;
    
    MOVETO(m_levelPos.x, m_levelPos.y);
    
    cout<<"等级："<<m_level<<flush;
    
    MOVETO(m_NextPos.x, m_NextPos.y);
    
    cout<<"下一关分数："<<NeedScore[m_level]<<flush;
}

int CSimpleSnake::ClearPrint(uint32_t x, uint32_t y, uint32_t len)
{
    for(int i = 0; i < len ; ++i)
    {
        MOVETO(x + i, y);  //暂时只支持行删除
        cout<<" "<<flush;
    }
    
    return 0;
}

int CSimpleSnake::EatFood()
{
    system("echo EatFood >> debug_log");
    //先将当前的食物消除
    ClearPrint(m_foodPos.x, m_foodPos.y, 1);
    
    m_snakebody.push_front(m_lastPos);  //新节点插入头部
    
    //修改分数，以及设置过关等级
    //设置每个食物加50分
    
    m_score += m_exp;
    
    //判断是否晋级
    if (m_score >= NeedScore[m_level])
    {
        m_level += 1;
        PrintImportInfo("Level UP");   
        
        if (MAX_LEVEL <= m_level)
        {
            PrintImportInfo("LEVEL IS MAX");    
            return 1;
        }
    }
    
    PrintCalcuteInfo();
    
    CreateFood();
    
    return 0;
}

int CSimpleSnake::PrintImportInfo(const char * msg)
{
    MOVETO(m_right/2 - 3, m_down/2);
    printf("\033[31m%s\033[5m", msg);
    
    printf("\033[0m");
    cout<<flush;
    
    sleep(3);
    
    //怎么删除这行的打印信息呢??
    int len = strlen(msg);
    
    ClearPrint(m_right/2 - 3, m_down/2, len);
    
    return 0;
}

int CSimpleSnake::GetTerminalSize()
{
    struct winsize size;
    
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    printf("%d\n",size.ws_col);
    printf("%d\n",size.ws_row);
    
    uint32_t info_size = 10;  //用于打印分数和等级信息
    
    //当横轴大于终端的列值时，取终端列值
    if (m_right > size.ws_col - info_size)
    {
        m_right = size.ws_col - info_size;
    }
    
    //当纵轴大于等于终端的行值时，取终端行值-1
    if (m_down > size.ws_row - info_size)
    {
        m_down = size.ws_row - info_size;
    }
    
    
    return 0;
}

int CSimpleSnake::DrawSnake()
{
    system("echo DrawSnake >> debug_log");
    //打印之前，清除一些提示
    
    //先打印蛇身，再打印蛇头。不然的话，当自身相撞时，蛇头的打印会被蛇身覆盖掉
    unsigned snake_size = m_snakebody.size();
    unsigned count = 0;
    char d = 79;
    
    for(list<TPos>::iterator titer = m_snakebody.begin(); titer != m_snakebody.end(); ++titer)
    {
        ++count;
        
        if (count >= snake_size)
        {
            break;
        }
     
        MOVETO(titer->x, titer->y);
        
        cout<<d<<flush;
    }
    
    //打印蛇头    
    char c = ' ';
    list<TPos>::reverse_iterator riter = m_snakebody.rbegin();
    
    //设置蛇头红色
        
    MOVETO(riter->x, riter->y);
    printf("\033[41;34m%c\033[0m", c);
    cout<<flush;

    return 0;
}

int CSimpleSnake::CreateFood()
{
    system("echo CreateFood >> debug_log");
    char ch = ' ';
    
    while(true)
    {
        int random_x = Common::GetRandomValue(m_right);
        int random_y = Common::GetRandomValue(m_down);
        
        //判断x,y是否已经存在物体，如蛇或者墙壁
        if (0 != CheckPosExist(random_x, random_y))
        {
            continue;
        }
        
        if (m_foodPos.x == random_x && m_foodPos.y == random_y)  //预防当前食物节点出现在上次相同的地方
        {
            continue; 
        }
        
        //打印食物
        #ifdef DEBUG        
        char msg[100] = {0};
        sprintf(msg, "echo %u %u >> food.pos", random_x, random_y);
        system(msg);
        
        MOVETO(m_DebugPos.x, m_DebugPos.y);
        cout<<"食物位置:"<<random_x<<","<<random_y<<"    "<<flush;

        
        #endif
        MOVETO(random_x, random_y);

        printf("\033[42;32m%c\033[0m", ch);
        cout<<flush;
        
        m_foodPos.x = random_x;
        m_foodPos.y = random_y;
        
        break;
    }
    
    return 0;
}

int CSimpleSnake::CheckPosExist(uint32_t x, uint32_t y)
{
    //先判断是否在墙上
    if (x >= m_right || x  <= m_left || y <= m_up || y >= m_down)  //
    {
        return -1;
    }
    
    //判断是否在蛇身上
    //循环蛇的所有坐标
    int match = 0;
    
    for(list<TPos>::iterator titer = m_snakebody.begin(); titer != m_snakebody.end(); ++titer)
    {
        if (x == titer->x && y == titer->y)
        {
            ++match;
        }
    }
    
    return match;
}

int CSimpleSnake::SwapSnake()
{
    //交换蛇的坐标值，像海浪一样，后浪把值给前浪。
    //交换链表的值
    list<TPos>::iterator lastiter = m_snakebody.begin();
    list<TPos>::iterator titer = m_snakebody.begin();
    ++titer;
    
    m_lastPos.x = lastiter->x;
    m_lastPos.y = lastiter->y;
    
    for(; titer != m_snakebody.end(); ++titer)
    {
        lastiter->x = titer->x;
        lastiter->y = titer->y;
        
        lastiter = titer;
    }
    
    return 0;
}

int CSimpleSnake::ClearSnake()
{
    for(list<TPos>::iterator titer = m_snakebody.begin(); titer != m_snakebody.end(); ++titer)
    {
        MOVETO(titer->x, titer->y);
        cout<<" "<<flush;
    }
}

int CSimpleSnake::CheckCollide()
{   
    //检查碰撞，暂时只是判断了是否撞墙
    list<TPos>::reverse_iterator riter = m_snakebody.rbegin();
    
    TPos & lastpos = *riter;
   
    int ret = CheckPosExist(lastpos.x, lastpos.y);
    
    if (-1 == ret || ret >= 2)  //-1时是撞墙，大于2时是撞自身
    {
        return 1;
    }
    
    //判断是否撞到食物
    if (m_foodPos.x == lastpos.x && m_foodPos.y == lastpos.y)
    {
        return 2;
    }
    
    return 0;
}

int CSimpleSnake::SetDiresction(uint8_t udir)
{
    //设置同一方向上的则不生效，比如已经向右运动，则向左向右均无效
    // 1 == 2   3 == 4   //==表示同方向 
    //const int DirectionCompare[4] = {0, 0, 1, 1};  //右左上下，判断是否在同一个方向。值相同，则方向相同
    
    if (DirectionCompare[udir - 1] == DirectionCompare[m_direction - 1])  //方向相同，不生效
    {
        cout<<"\a\b"<<flush;
        return 1;    
    }        
     
    //怎么才能立即生效?信号处理? 
    
    
    while(true)
    {
        if (m_isAllowChange)
        {
            m_direction = udir;
            m_isAllowChange = false;
            break;
        }
    }

    return 0;
}

int CSimpleSnake::GameEnd()
{
    MOVETO(0, m_down+2);
    
    SHOW_CURSOR();

    //主进程死亡时，子线程自然死亡
    cout<<"Game Over"<<endl;
    
    
    close_keyboard();
    exit(0);
}

void * CSimpleSnake::ListenKeybordThread(void * vpParam)
{
    CSimpleSnake * psnake = (CSimpleSnake *)vpParam;
    
    char ch;
    
    kbhit();
        
    while(true)
    {
        int nread;
        
        ch = readch();
        
        uint8_t dire = 0;
        
        switch(ch)
        {
            case RIGHT: dire = DIRECTION_RIGHT; break;
            case LEFT: dire = DIRECTION_LEFT; break;
            case UP: dire = DIRECTION_UP; break;
            case DOWN: dire = DIRECTION_DOWN; break;
            
            case CTRLC : psnake->GameEnd();
            default : break;
        }
        
        if (dire != 0)
        {
            psnake->SetDiresction(dire);
            //怎么让方向改变后，立即生效?
        }
        
    }
    
    return (void *)0;
}

int StringToUInt(const char * ptr, uint32_t & result)
{
    //use byte convert;
    result = 0;
    
    int len = strlen(ptr);
    
    for (int i = 0; i < len; ++i)
    {
        result *= 10;
        result += ptr[i]-'0'; 
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    init_keyboard();
    
    uint32_t x =0, y = 0;
    
    if (3 != argc)
    {
        x = 30;
        y = 30;
    }
    else
    {
        StringToUInt(argv[1], x);
        StringToUInt(argv[2], y);
    }
    
    #ifdef DEBUG
        system("rm -f food.pos");
        system("rm -f debug_log");
    #endif
       
    CSimpleSnake simpledemo;
    
    simpledemo.Init(x, y);
    
    simpledemo.Run();
    
    
    return 0;
}
