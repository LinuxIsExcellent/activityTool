#ifndef LST_TIMER
#define LST_TIMER

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<pthread.h>

#define BUFFER_SIZE 64
class util_timer
{
    public:
        util_timer():prev(NULL),next(NULL),nLoopSec(0),nLoopMax(0){}
    public:
        time_t expire;
        unsigned int nLoopSec;  /*循环间隔如果大于0， 则证明这个定时器是一个多次循环的定时器*/
        unsigned int nLoopMax;  /*循环次数上限，等于0为无限次*/
        unsigned int nLoopcount;  /*当前已经循环了的次数，等于0为无限次*/
        void (*cb_func)(); /*任务回调函数*/
        util_timer* prev;
        util_timer* next;
};

class sort_timer_lst
{
    public:
        sort_timer_lst():head(NULL), tail(NULL){}

        ~sort_timer_lst()
        {
            util_timer* tmp = head;
            while(tmp)
            {
                head = tmp->next;
                delete tmp;
                tmp = head;
            }
        }

        void add_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }

            if (!head)
            {
                head = tail = timer;
                return;
            }

            if (timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }

            add_timer(timer, head);
        }

        void adjust_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }

            util_timer* tmp = timer->next;

            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }

            if (timer == head)
            {
                head = head->next;
                head->prev = NULL;
                timer->next = NULL;
                add_timer(timer, head);
            }
            else
            {
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                add_timer(timer, timer->next);
            }
        }

        void del_timer(util_timer * timer)
        {
            if (!timer)
            {
                return;
            }

            if ((timer == head)&& (timer ==tail))
            {
                delete timer;
                head = NULL;
                tail = NULL;
                return;
            }

            if (timer == head)
            {
                head = head->next;
                head->prev = NULL;
                delete timer;
                return ;
            }

            if (timer == tail)
            {
                tail = tail->prev;
                tail->next = NULL;
                delete timer;
                return;
            }

            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
        }

    
        void tick()
        {
            if (!head)
            {
                return;
            }

            printf("timer tick \n");

            time_t cur = time(NULL);

            util_timer* tmp = head;

            while(tmp)
            {
                if (cur < tmp ->expire)
                {
                    break;
                }
                tmp->cb_func();
                head=tmp->next;

                if (head)
                {
                    head->prev = NULL;
                }

                delete tmp;
                tmp = head;
            }
        }
    private:
        void add_timer(util_timer* timer, util_timer* lst_head)
        {
            util_timer* prev = lst_head;
            util_timer* tmp = prev->next;

            while(tmp)
            {
                if(timer->expire < tmp->expire)
                {
                    prev->next = timer;
                    timer->next = tmp;
                    tmp->prev = timer;
                    timer->prev = prev;
                    break;
                }
                prev = tmp;
                tmp = tmp->next;
            }

            if (!tmp)
            {
                prev->next = timer;
                timer->prev = prev;
                timer->next = NULL;
                tail=timer;
            }
        }

    private:
        util_timer* head;
        util_timer* tail;
};

#endif
