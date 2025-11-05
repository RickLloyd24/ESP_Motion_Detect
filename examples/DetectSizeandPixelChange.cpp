#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

/**
 * Instantiate motion detector and camera objects
 */
MotionDetect::Motion motion;
MotionDetect::Camera camera;

/**
 *
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("Start", "Setup starting ...");

    /* Configure motion detection parameters */
    motion.setMinChanges(10);           //Set minimum number of pixel changes to 10 to trigger motion
    motion.setMinPixelDiff(20.0);       //Set minimum percent of pixels changes to 20% to trigger motion
    motion.setMinBufSizeChanges(10.0);  //Set minimum percent of frame size change to 10% to trigger motion
    motion.setMotionCount(2);           //Set to 2 to check for both pixel change and buffer size change

    /* Configure the camera */
    camera.wrover();                    //Set camera to Wrover-Kit board

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
    ESP_LOGI("End", "Setup complete");
    ESP_LOGI("Move", "Make Motions in front of the camera!");
}

void loop() {
    static int loopctr = 0;

    if (!camera.capture()) {
        ESP_LOGW("loop", "camera capture Failure");
        delay(1000);
        return;
    }
    unsigned long startTime = millis();
    int status = motion.detect(camera.getFrameBufptr(), camera.getFrameSize());
    if (status != 0) {
        unsigned long endTime = millis();
        Serial.printf("Status: %d, Percent Pixels: %5.2f%%, Percent Size: %5.2f%%, Execution Time: %lu ms\n", status, motion.getPercentDiff(), motion.getBufSizeChange(), endTime - startTime);
    }
    delay(100);

    if (loopctr++ > 100) {
        ESP_LOGI("Done", "End of test");
        delay(5000); while(1);
    }
    
}