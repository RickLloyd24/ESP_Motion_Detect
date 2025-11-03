#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>

MotionDetect::Camera camera;

void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("setup", "Start");

    camera.wrover();
    
    /* Change Configure of the wrover camera before camera.begin() */
    camera.setFrameSize(FRAMESIZE_SVGA);                    // Set frame size (FRAMESIZE_QVGA, FRAMESIZE_VGA, etc.)
    camera.setJpegQuality(30);                              // Set JPEG quality (0-63, lower = higher quality)
    camera.setFrameBufferLocation(CAMERA_FB_IN_DRAM);       // Set frame buffer location (PSRAM/DRAM)
    camera.setFrameBufferCount(1);                          // Set number of frame buffers
    camera.setPixelFormat(PIXFORMAT_JPEG);                  // Set pixel format (JPEG, RGB565, etc.)
    camera.setGrabMode(CAMERA_GRAB_WHEN_EMPTY);             // Set grab mode (LATEST, WHEN_EMPTY)
    camera.setXclkFreq(20000000);                           // Set XCLK frequency

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }    
        
// Now you can adjust sensor settings at runtime 
    sensor_t * s = esp_camera_sensor_get();
        s->set_brightness(s, 1);                  //-2 to 2 Adjusts the image brightness. Values are typically in the range of.
        s->set_contrast(s, 1);                    // -2 to 2 Adjusts the image contrast.
        s->set_saturation(s, 0);                  // -2 to 2 Adjusts the image saturation.
        s->set_vflip(s, 1);                       // 0 = disable , 1 = enable Vertical Flip the image
        s->set_hmirror(s, 1);                     // 0 = disable , 1 = enable Horizonal mirror the image

    camera.printSensorValues();
    
}

void loop() {
    if (!camera.capture()) {
        ESP_LOGW("loop", "camera capture Failure");
        delay(1000);
        return;
    }
    else {
        ESP_LOGI("loop", "Camera capture OK");
    }
    delay (5000); // wait 5 seconds before next capture
}