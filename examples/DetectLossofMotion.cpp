#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

/**
 * Instantiate motion detector and camera objects
 */
MotionDetect::Motion motion;
MotionDetect::Camera camera;

boolean motionDetected = false;
unsigned long motionDetectedAt = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("setup", "Start");

    /* Configure motion detection parameters */
    motion.setMinChanges(10);            //Set minimum number of pixel changes to 10 to trigger motion
    motion.setMinPixelDiff(20.0);        //Set minimum percent of pixels that must change to 20% to trigger motion
    motion.setMinBufSizeChanges(100.0);  //Disable buffer size change detection

    /* Configure the camera */
    camera.wrover();                  //Set camera to Wrover-Kit board 

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
    /* Update camera sensor settings */
    setCamSensor->set_brightness(setCamSensor, 1);    //increase brightness by 1 level
    setCamSensor->set_vflip(setCamSensor, 1);         // flip the picture vertically
}

void loop() {
    static int loopctr = 0;

    if (!camera.capture()) {
        ESP_LOGW("loop", "camera capture Failure");
        delay(1000);
        return;
    }
    if (motion.detect(camera.getFrameBufptr(), camera.getFrameSize())) {
        motionDetectedAt = millis();
        if (!motionDetected) {
            motionDetected = true;
            ESP_LOGI("Start", "Motion detected!, percent change: %f", motion.getPercentDiff());
        }
    }   
    if (millis() - motionDetectedAt > 2000 && motionDetected) {
        motionDetected = false;
        ESP_LOGI("End", "No motion for 2 seconds, percent change: %5.2f", motion.getPercentDiff());
        motionDetected = false;
    }
    if (loopctr++ > 1000) {
        ESP_LOGI("Done", "End of test");
        delay(5000); while(1);
    }  
}