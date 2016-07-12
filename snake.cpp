#include "snake.h"
#include "kbinit.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <pthread.h>


using namespace std;

CSimpleSnake::CSimpleSnake():m_direction(DIRECTION_RIGHT),m_left(1),m_right(1),m_up(1),m_down(1)
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
   
   
    GetTerminalSize();

    DrawBox(' ');
    
    /*
    m_lastPos.x = m_left + 1;
    m_lastPos.y = m_up + 1;
    
    */
    
    TPos snake;
    
    //测试，假设蛇的长度是3
    snake.x = m_left + 1;
    snake.y = m_up + 1;
    
    m_snakebody.push_front(snake);
    
    snake.x += 1;
    m_snakebody.push_front(snake);
    
    snake.x += 1;
    m_snakebody.push_front(snake);
    
    snake.x += 1;
    m_snakebody.push_front(snake);
    
    DrawSnake();
    
    
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
    int times = 0;
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
    
    while(true)
    {
        if (times > 1000)
        {
            break;
        }
        
        ret = Move();
        
        if (ret)
        {
            break;
        }
        //break;
        
        ++times;
        
        usleep(1000*300);
        //sleep(1);
    }
    
    GameEnd();

    return 0;
}

int CSimpleSnake::Move()
{
    //实现移动的效果，就是不断的重绘
    
    //先清除旧的位置，再打印新的位置
    /*
    MOVETO(m_lastPos.x +1, m_lastPos.y);
    cout<<"\b "<<flush;
    */
    ClearSnake();
    SwapSnake();
    
    //设置每次循环移动2
    uint32_t speed = 1;

    //修改尾部指针的坐标即可
    
    list<TPos>::reverse_iterator riter = m_snakebody.rbegin();
    
    TPos & changepos = *riter;
    
    switch(m_direction)
    {
        case DIRECTION_RIGHT: changepos.x += speed; break;
        case DIRECTION_LEFT: changepos.x -= speed; break;
        case DIRECTION_UP: changepos.y -= speed; break;
        case DIRECTION_DOWN: changepos.y += speed; break;
    }

    int ret = CheckCollide();
    
    if (ret)
    {
        return ret;
    }
    
    DrawSnake();
    
    return 0;
}


int CSimpleSnake::DrawSnake()
{
    /*
    MOVETO(m_lastPos.x, m_lastPos.y);
        
    char c = 4;
    printf("\033[41;34m%c\033[0m", c);
    
    cout<<flush;
    */
    
    char c = 4;
    
    for(list<TPos>::iterator titer = m_snakebody.begin(); titer != m_snakebody.end(); ++titer)
    {
        MOVETO(titer->x, titer->y);
        printf("\033[41;34m%c\033[0m", c);
    
        cout<<flush;
    }
    
    return 0;
}

int CSimpleSnake::SwapSnake()
{
    //交换链表的值
    list<TPos>::iterator lastiter = m_snakebody.begin();
    list<TPos>::iterator titer = m_snakebody.begin();
    ++titer;
    
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
    list<TPos>::reverse_iterator riter = m_snakebody.rbegin();
    
    TPos & lastpos = *riter;
    
    if (lastpos.x >= m_right || lastpos.x  <= m_left || lastpos.y <= m_up || lastpos.y >= m_down)  //
    {
        return 1;
    }
    
    return 0;
}

int CSimpleSnake::SetDiresction(uint8_t udir)
{
    m_direction = udir;
    
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
        
        //char buf[3];
        
        //cout<<"开始"<<endl;
        
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
        }
        
    }
    
    return (void *)0;
}

int CSimpleSnake::GetTerminalSize()
{
    struct winsize size;
    
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    printf("%d\n",size.ws_col);
    printf("%d\n",size.ws_row);
    
    //当横轴大于终端的列值时，取终端列值
    if (m_right > size.ws_col)
    {
        m_right = size.ws_col;
    }
    
    //当纵轴大于等于终端的行值时，取终端行值-1
    if (m_down > size.ws_row)
    {
        m_down = size.ws_row - 1;
    }
    
    return 0;
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
    
    CSimpleSnake simpledemo;
    
    simpledemo.Init(x, y);
    
    simpledemo.Run();
    
    
    return 0;
}
