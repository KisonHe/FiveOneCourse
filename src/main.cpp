// Example for library:
// https://github.com/Bodmer/TJpg_Decoder

// This example if for an ESP8266 or ESP32, it renders a Jpeg file
// that is stored in a SPIFFS file. The test image is in the sketch
// "data" folder (press Ctrl+K to see it). You must upload the image
// to SPIFFS using the ESP8266 or ESP32 Arduino IDE upload menu option.

// Include the jpeg decoder library

#include <TJpg_Decoder.h>
// #include "playAnime.h"
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

#include "HCSR04Task.h"
#include "MusicTask.h"
#include "ESP32MotorControl.h"

ESP32MotorControl MotorControl = ESP32MotorControl();


// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

#include <lvgl.h>
#include <TFT_eSPI.h>
// #include "lv_demo_widgets.h"
SemaphoreHandle_t LVGL_Semaphore;
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
lv_obj_t * distancelabel = nullptr;
lv_obj_t * timelabel = nullptr;
uint32_t timeStart = 0;
uint32_t timeStop = 0;
static lv_style_t style1;
lv_obj_t * reverseLED = nullptr;

/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch(&touchX, &touchY, 600);

    if(!touched) {
      data->state = LV_INDEV_STATE_REL;
    } else {
      data->state = LV_INDEV_STATE_PR;
	    
      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;
  
      Serial.print("Data x");
      Serial.println(touchX);
      
      Serial.print("Data y");
      Serial.println(touchY);
    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/* Reading input device (simulated encoder here) */
bool read_encoder(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    static int32_t last_diff = 0;
    int32_t diff = 0; /* Dummy - no movement */
    int btn_state = LV_INDEV_STATE_REL; /* Dummy - no press */

    data->enc_diff = diff - last_diff;;
    data->state = btn_state;

    last_diff = diff;

    return false;
}

int deleteb = 0;
static void k_b_event_handler(lv_obj_t * obj, lv_event_t event)
{
    deleteb = 1;
}
lv_obj_t * btn1 = NULL;
void lv_ex_btn_1(void)
{
    lv_obj_t * label;

    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, k_b_event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Start");
}
lv_obj_t * gauge1 = nullptr;
void lv_ex_gauge_1(void)
{
    /*Describe the color for the needles*/
    static lv_color_t needle_colors[1];
    needle_colors[0] = LV_COLOR_BLUE;

    /*Create a gauge*/
    gauge1 = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_range(gauge1, -120, 120);
    lv_gauge_set_needle_count(gauge1, 1, needle_colors);
    lv_obj_set_size(gauge1, 200, 200);
    lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Set the values*/
    lv_gauge_set_value(gauge1, 0, 0);
}


void lv_reverse_led_init(void)
{
    /*Create a LED and switch it OFF*/
    reverseLED = lv_led_create(lv_scr_act(), NULL);
    lv_obj_align(reverseLED, NULL, LV_ALIGN_CENTER, 100, 70);
    lv_led_off(reverseLED);
    // lv_led_on(reverseLED);
}

void lv_ex_label_1(void)
{
    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_text(label1, "#0000ff Hello！#");
    lv_obj_set_width(label1, 150);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_add_style(label1, LV_BTN_PART_MAIN, &style1);

    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SROLL_CIRC);     
    lv_obj_set_width(label2, 100);
    lv_label_set_text(label2, "ID:2018040704009");
    lv_obj_align(label2, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 30);
    lv_obj_add_style(label2, LV_BTN_PART_MAIN, &style1);

    distancelabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(distancelabel, LV_LABEL_LONG_SROLL_CIRC);     
    lv_obj_set_width(distancelabel, 100);
    lv_label_set_text_fmt(distancelabel, "%f", distance);
    lv_obj_align(distancelabel, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    timelabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(timelabel, LV_LABEL_LONG_SROLL_CIRC);     
    lv_obj_set_width(timelabel, 100);
    lv_label_set_text_fmt(timelabel, "%d", 0);
    lv_obj_align(timelabel, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    // lv_obj_add_style(distancelabel, LV_BTN_PART_MAIN, &style1);
}

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}


void setup()
{
  LVGL_Semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(LVGL_Semaphore);
  Serial.begin(115200);
  Serial.println("\n\n Testing TJpg_Decoder library");
  MotorControl.attachMotors(14, 15, 16, 17);

  // // Initialise SPIFFS
  // if (!SPIFFS.begin()) {
  //   Serial.println("SPIFFS initialisation failed!");
  //   while (1) yield(); // Stay here twiddling thumbs waiting
  // }
  // Serial.println("\r\nInitialisation done.");

  // Initialise the TFT
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  // TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  // TJpgDec.setCallback(tft_output);
  // playjpgAnime_config_t bootAnimeConfig = {
  //       .name = "/out",
  //       .startnum = 0,
  //       .endnum = 40,
  //       .numlen = 3,
  //       .x = 0,
  //       .y = 0,
  //   };
  // while(playjpgAnime(&bootAnimeConfig))
  //   vTaskDelay(2);

  // while(playjpgAnime("/anime",0,328,3))
  //   vTaskDelay(1);
    xSemaphoreTake(LVGL_Semaphore,portMAX_DELAY);
    lv_init();
    uint16_t calData[5] = { 376, 3526, 288, 3524, 7 };
    tft.setTouch(calData);
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (real) input device driver*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    lv_ex_btn_1();
    lv_ex_label_1();
    lv_reverse_led_init();
    xSemaphoreGive(LVGL_Semaphore);
    // test_app_main();
    xTaskCreatePinnedToCore((TaskFunction_t) HCSR04_Task,
            (const char *) "HCSR04Task",
            (uint16_t) 4096,
            (void *) NULL,
            (UBaseType_t) 1,
            (TaskHandle_t *) &HCSR04_Task_Handle,
            tskNO_AFFINITY); 
    xTaskCreatePinnedToCore((TaskFunction_t) Music_Task,
            (const char *) "MusicTask",
            (uint16_t) 4096,
            (void *) NULL,
            (UBaseType_t) 1,
            (TaskHandle_t *) &Music_Task_Handle,
            tskNO_AFFINITY);    

}

void loop()
{
    xSemaphoreTake(LVGL_Semaphore,portMAX_DELAY);
    lv_label_set_text_fmt(distancelabel, "%f", distance);
    lv_task_handler(); /* let the GUI do its work */
    if (deleteb){
      timeStart = millis();
      deleteb = 0;
      lv_obj_del_async(btn1);
      lv_ex_gauge_1();
    }
    // static uint8_t i = 0;
    // i++;
    // i = i > 10 ? 0 : i;
    // if (!i)
    //   carSpeed += 1;
    // carSpeed = carSpeed >= 120 ? 0 :carSpeed;
    lv_label_set_text_fmt(timelabel, "%d", timeStop-timeStart);
    if (gauge1 != nullptr)
    {
      // static int16_t lastcarSpeed = 0;
      // if (lastcarSpeed != carSpeed){
        // lastcarSpeed = carSpeed;
        
        lv_gauge_set_value(gauge1, 0, carSpeed);
      // }
    }
    xSemaphoreGive(LVGL_Semaphore);
    vTaskDelay(5);
}
