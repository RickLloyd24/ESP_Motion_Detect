# ESP Camera Motion Detector

This is a simple library to help setup an ESP32 with a camera and to detect motion in the camera frame.

The library uses the picojpeg library by Rich Geldreich <richgel99@gmail.com> to decode a JPG file for motion detection.

The library includes standard configurations for multiple boards for quick and easy configurartion.  The library also includes custom configuration of the board and camera for boards that are not supported.

The library sets common sensor settings and also allows modification any of the settings.

This library uses two methods to detect motion: pixel changes in the jpeg buffer and jpeg buffer size change.  There is an option to use either or both.  The pixel changes is much more accurate.  The buffer size chnange is more accurate with larger buffers.  The following show approximate execution time to detect pixel changes on a WROVER processor:
    320x240   -  12 milliseconds
    640x480   -  36 milliseconds
    800x600   -  58 milliseconds
    1280x1024 - 197 milliseconds

## Code Example

#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

 /* Instantiate motion detector and camera objects */
MotionDetect::Motion motion;
MotionDetect::Camera camera;

boolean motionDetected = false;
unsigned long motionDetectedAt = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    /* Configure the camera */
    camera.wrover();

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
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

## Hardware Support

This library should support the following boards:  aithinker, m5, m5wide, eye, wrover and ESP32-CAM.

 The library was tested on ESP32-CAM and Freenove WROVER.

## Similar libraries

Eloquent Surveillance (archived)
Eloquent Esp32 cam (camera support only)
