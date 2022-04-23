#ifndef LST_TIMER
#define LST_TIMER

#define BUFFER_SIZE 64
class util_timer
{
    public:
        util_timer():prev(NULL),next(NULL),expire(0),nLoopSec(0),nLoopMax(0),nLoopcount(0),cb_func(NULL){}
    public:
        time_t expire;  /*定时器截止时间*/
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

            time_t cur = time(NULL);


            util_timer* first_time = head;

            while(first_time)
            {
                if (cur < first_time ->expire)
                {
                    break;
                }

                //调用回调函数
                first_time->cb_func();
                
                //printf("nLoopSec = %d, nLoopMax = %d, first_time->nLoopcount = %d, first_time ->expire = %d\n", first_time->nLoopSec, first_time->nLoopMax, first_time->nLoopcount, first_time->expire);
                if (first_time->nLoopSec > 0 && (first_time->nLoopMax == 0 || first_time->nLoopcount < first_time->nLoopMax - 1))
                {
                    // 修改这个循环定时器的参数
                    first_time->nLoopcount += 1;
                    //为了避免服务器调整时间之后，多次同一个回调函数的调用
                    first_time->expire = cur + first_time->nLoopSec;

                    // 1.后续没有定时器
                    // 2.调整了时间之后还是小于后续的定时器截至时间
                    // 则直接退出循环
                    if(first_time->next == NULL || (first_time->expire <= first_time->next->expire))
                    {
                        // printf("直接退出循环, first_time = %x, first_time->expire = %d, first_time->next->expire = %d\n", first_time, first_time->expire, first_time->next->expire);
                        break;
                    }

                    //调整头指针的位置
                    head = first_time->next;
                    head->prev = NULL;

                    util_timer* temp_next = first_time->next;
                    while(temp_next->expire < first_time->expire)
                    {
                        if (first_time->expire < temp_next->expire)
                        {
                            temp_next->prev->next = first_time;
                            temp_next->prev = first_time;

                            first_time->prev = temp_next->prev->next;
                            first_time->next = temp_next;

                            break;
                        }

                        if (temp_next->next)
                        {
                            temp_next = temp_next->next;
                        }
                        else
                        {
                            temp_next->next = first_time;
                            first_time->prev = temp_next;
                            first_time->next = NULL;

                            tail = first_time;
                        }
                    }
                }
                // 如果定时器已失效
                else
                {
                    head = first_time->next;
                    if (head)
                    {
                        head->prev = NULL;
                    }

                    delete first_time;
                    first_time = head->next;
                }
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
