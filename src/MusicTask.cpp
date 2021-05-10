#include <Arduino.h>
#include "MusicTask.h"
#include <lvgl.h>

TaskHandle_t Music_Task_Handle = nullptr;
uint8_t MusicTaskReverse = 0;

extern lv_obj_t * reverseLED;

extern "C"{
    void play_reverse();
    void wav_player_init();
}

void Music_Task(void* pvParameters)
{
    wav_player_init();
    while (1)
    {
        if (MusicTaskReverse){
            lv_led_on(reverseLED);
            play_reverse();
        }
        else{
            lv_led_off(reverseLED);
        }
        vTaskDelay(50);
    }
}
