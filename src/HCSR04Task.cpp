#include <Arduino.h>
#include "HCSR04Task.h"
#include <HCSR04.h>
#include "pid.h"
#include <lvgl.h>
#include "ESP32MotorControl.h"
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; 
extern uint8_t MusicTaskReverse;
extern ESP32MotorControl MotorControl;
static const float WEIGHT = 0.5;
static int16_t speedAVG = 0;
// FIXME
static const float STOP_DISTANCE = 15;
static const float tolerate = 3;
pid speedPID(0.5, 0.4, 0, 15, 40);
extern lv_obj_t *gauge1;
int16_t carSpeed = 0;
double distance = 0;

extern uint32_t timeStop;

TaskHandle_t HCSR04_Task_Handle = nullptr;
void HCSR04_Task(void *pvParameters)
{
    portENTER_CRITICAL(&timerMux);
    UltraSonicDistanceSensor distanceSensor(25, 26);
    portEXIT_CRITICAL(&timerMux);
    while (1)
    {
        distance = distanceSensor.measureDistanceCm();
        if (gauge1 != nullptr)
        {
            static int8_t fuckCNT = 0;
            
            if (distance < 0)
            {
                //handle it
                fuckCNT++;
                if (fuckCNT > 10)
                {
                    MusicTaskReverse = 0;
                    carSpeed = 0;
                    Serial.println("Oh shit...");
                }
            }
            else if (distance > 35)
            {
                fuckCNT = 0;
                carSpeed = 40;
            }
            else
            {
                fuckCNT = 0;
                if (fabs(distance - STOP_DISTANCE) > tolerate)
                    speedAVG = speedAVG * (1 - WEIGHT) + speedPID.pid_run(distance - STOP_DISTANCE);
                else
                {
                    if (timeStop == 0)
                    {
                        timeStop = millis();
                    }
                    speedAVG = 0;
                }
                if (speedAVG > 100)
                {
                    speedAVG = 100;
                }
                if (speedAVG < -100)
                {
                    speedAVG = -100;
                }
                carSpeed = speedAVG;
            }
            if (carSpeed > 100)
            {
                carSpeed = 100;
            }
            if (carSpeed < -100)
            {
                carSpeed = -100;
            }
            if (carSpeed >= 0)
            {
                MusicTaskReverse = 0;
                MotorControl.motorForward(0, carSpeed);
                MotorControl.motorForward(1, carSpeed);
            }
            else if (carSpeed < 0)
            {
                MusicTaskReverse = 1;
                MotorControl.motorReverse(0, -carSpeed);
                MotorControl.motorReverse(1, -carSpeed);
            }
        }
        else{
            MotorControl.motorForward(0, 0);
            MotorControl.motorForward(1, 0);
        }
        Serial.print("Distance:");
        Serial.println(distance);
        Serial.print("carSpeed:");
        Serial.println(carSpeed);
        vTaskDelay(50);
    }
}
