#ifndef HCSR04TASK_H
#define HCSR04TASK_H

extern TaskHandle_t HCSR04_Task_Handle;

extern int16_t carSpeed;
extern double distance;

void HCSR04_Task(void* pvParameters);
#endif