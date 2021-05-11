#ifndef _TIMER_HPP_
#define _TIMER_HPP_
/* 
#include<time.h>
#include<stdio.h>
int main()
{
    double dur;
    clock_t start,end;
    start = clock();
    foo();//dosomething
    end = clock();
    dur = (double)(end - start);
    printf("Use Time:%f\n",(dur/CLOCKS_PER_SEC));
}
 */

#include <bits/stdc++.h>

struct Timer
{
    clock_t time_start;
    clock_t time_end;
    void Start() // 开始计时
        ;
    double End() // 结束计时
        ;
    Timer();
    double Current();
    double GetTime() // 获取计时结果
        ;
    void Print() // 直接打印计时结果
        ;
};

#endif