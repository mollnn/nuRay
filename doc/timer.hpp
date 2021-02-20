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
    {
        time_start = clock();
    }
    double End() // 结束计时
    {
        time_end = clock();
        return GetTime();
    }
    Timer()
    {
        Start();
    }
    double Current()
    {
        double ans = (clock() - time_start) * 1.0 / CLOCKS_PER_SEC;
        return ans;
    }
    double GetTime() // 获取计时结果
    {
        double ans = (time_end - time_start) * 1.0 / CLOCKS_PER_SEC;
        return ans;
    }
    void Print() // 直接打印计时结果
    {
        double ans = (time_end - time_start) * 1.0 / CLOCKS_PER_SEC;
        printf("%.6f", ans);
    }
};

#endif