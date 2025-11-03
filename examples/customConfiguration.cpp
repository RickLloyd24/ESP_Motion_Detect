#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

/**
 * Instantiate motion detector and camera objects
 */
MotionDetect::Camera camera;

camera_config_t * CamConfig = nullptr;

void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("setup", "Start");

    /* Configure the wrover camera before camera.begin() */
    CamConfig = new camera_config_t;
    CamConfig->pin_d0 = 4;
    CamConfig->pin_d1 = 5;
    CamConfig->pin_d2 = 18;
    CamConfig->pin_d3 = 19;
    CamConfig->pin_d4 = 36;
    CamConfig->pin_d5 = 39;
    CamConfig->pin_d6 = 34;
    CamConfig->pin_d7 = 35;
    CamConfig->pin_xclk = 21;
    CamConfig->pin_pclk = 22;
    CamConfig->pin_vsync = 25;
    CamConfig->pin_href = 23;
    CamConfig->pin_sccb_sda = 26;
    CamConfig->pin_sccb_scl = 27;
    CamConfig->pin_pwdn = -1;
    CamConfig->pin_reset = -1;
    CamConfig->xclk_freq_hz = 20000000;
    CamConfig->ledc_timer = LEDC_TIMER_0;
    CamConfig->ledc_channel = LEDC_CHANNEL_0;
    CamConfig->pixel_format = PIXFORMAT_JPEG;      //YUV422,GRAYSCALE,RGB565,JPEG
    CamConfig->frame_size = FRAMESIZE_VGA;         //QQVGA,QQVGA2,QVGA,CIF,VGA,SVGA,XGA,SVGA,UXGA
    CamConfig->jpeg_quality = 30;                  //0-63 lower number means higher quality
    CamConfig->fb_count = 1;                       //if more than one, it will use continuous mode
    CamConfig->fb_location = CAMERA_FB_IN_PSRAM;   //CAMERA_FB_IN_PSRAM or CAMERA_FB_IN_DRAM
    CamConfig->grab_mode = CAMERA_GRAB_WHEN_EMPTY; //CAMERA_GRAB_WHEN_EMPTY or CAMERA_GRAB_LATEST

    camera.config(*CamConfig);

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
}
void loop() {
    if (!camera.capture()) {
        ESP_LOGW("loop", "camera capture Failure");
        delay(1000);
        return;
    }
    else {
        ESP_LOGI("loop", "Camera capture OK");

        // do something with the picture here...
    }

    delay (5000); // wait 5 seconds before next capture
}
