#include "timer.hpp"

void Timer::Start() // 开始计时
{
    time_start = clock();
}
double Timer::End() // 结束计时
{
    time_end = clock();
    return GetTime();
}
Timer::Timer()
{
    Start();
}
double Timer::Current()
{
    double ans = (clock() - time_start) * 1.0 / CLOCKS_PER_SEC;
    return ans;
}
double Timer::GetTime() // 获取计时结果
{
    double ans = (time_end - time_start) * 1.0 / CLOCKS_PER_SEC;
    return ans;
}
void Timer::Print() // 直接打印计时结果
{
    double ans = (time_end - time_start) * 1.0 / CLOCKS_PER_SEC;
    printf("%.6f", ans);
}