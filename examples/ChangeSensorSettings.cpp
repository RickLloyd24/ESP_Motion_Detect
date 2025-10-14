#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

MotionDetect::Camera camera;

void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("Start", "Serial started");

    /* Configure the camera */
    camera.aithinker();

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
    /* Update camera sensor settings after camera.begin*/
    setCamSensor->set_quality(setCamSensor, 30);                 //Sets the JPEG compression to low quality 
    setCamSensor->set_framesize(setCamSensor, FRAMESIZE_QVGA);   //Sets the frame size to QVGA 320x240
    setCamSensor->set_vflip(setCamSensor, 1);                    //verically flip the picture
    setCamSensor->set_brightness(setCamSensor, 1);               //increase the brightness by 1 level
    
    ESP_LOGI("End", "Setup Complete");
}

void loop() {
    if (!camera.capture()) {
        ESP_LOGW("loop", "camera capture Failure");
        delay(1000);
        return;
    }
    else {
        ESP_LOGI("loop", "Camera capture OK");

        // do something with the picture
    }

    delay (5000); // wait 5 seconds before next capture
}
