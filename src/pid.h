#ifndef PID_H
#define PID_H

#include "stdint.h"

//PID可以算pid输出 可以配置pid
class pid
{
public:
    //公共函数
    pid(float P, float I, float D, float IMax, float PIDMax, uint16_t I_Time = 1, uint16_t D_Time = 1, uint16_t I_Limited = 9999); //传统pid构造函数
    float pid_run(float err);
    void clearError();

private:
    uint32_t getSystemTick();
    float *Custom_Diff = nullptr; //!<自定义的外界浮点型微分数值 常用于路程环的微分环节(即速度值)、陀螺仪反馈值效果一般比编码器好
    float P;
    float I;
    float D;
    float IMax;
    float PIDMax;
    uint16_t I_Time;              
    uint16_t D_Time;              
    uint16_t I_Limited;           //!<当误差小于I_Limited时才进行I输出 plus专属
    float Pout;
    float Iout;
    float Dout;
    float Dout_Accumulative; //因为有微分时间重定义,因此加入一个变量真正用于输出D,源Dout用作累积误差
    float PIDout;
    float CurrentError;
    float LastError;
    uint32_t I_start_time; //!<积分开始时间戳，用于带时间参数的pid   plus专属
    uint32_t D_start_time; //!<微分开始时间戳，用于带时间参数的pid
};

#endif