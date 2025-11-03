#include <Arduino.h>
#include "MotionDetect.h"
#include <esp_log.h>  

MotionDetect::Camera camera;

void setup() {
    Serial.begin(115200);
    delay(3000);
    ESP_LOGI("Start", "Serial started");

    /* Configure the camera */
    camera.wrover();

    /* Initialize the camera */
    if (!camera.begin()) {
        ESP_LOGE("Setup", "camera begin failure");
    }
    else {
        ESP_LOGI("SUCCESS", "Camera OK");
    }
    ESP_LOGI("Debug", "Printing current sensor settings:");
    camera.printSensorValues();
    /* Update camera sensor settings after camera.begin*/
    sensor_t * s = esp_camera_sensor_get();
        s->set_ae_level(s, 0);                    // -2 to 2 Adjusts the auto exposure level
        s->set_aec_value(s, 168);                 // 0 to 1200 Sets the manual exposure value when AEC is disabled
        s->set_aec2(s, 1);                        // 0 = disable , 1 = enable Enables or disables AEC 2, an enhanced AEC algorithm
        s->set_agc_gain(s, 5);                    // 0 to 30  Sets the manual gain value when AGC is disabled.
        s->set_awb_gain(s, 1);                    // 0 = disable , 1 = enable Enables or disables white balance gain control
        s->set_bpc(s, 0);                         // 0 = disable , 1 = enable Enables or disables black pixel correction
        s->set_brightness(s, 0);                  //-2 to 2 Adjusts the image brightness. Values are typically in the range of.
        s->set_colorbar(s, 0);                    // 0 = disable , 1 = enable Enables a test color bar pattern for debugging
        s->set_contrast(s, 0);                    // -2 to 2 Adjusts the image contrast.
        s->set_dcw(s, 1);                         // 0 = disable , 1 = enable  Enables or disables downsize cropping.
        s->set_exposure_ctrl(s, 1);               // 0 = disable , 1 = enable Enables or disables automatic exposure control (AEC)
        s->set_gain_ctrl(s, 1);                   // 0 = disable , 1 = enable Enables or disables automatic gain control (AGC).
        s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6 Sets the maximum allowed gain when AGC is enabled. This can be used to control noise levels.
        s->set_hmirror(s, 1);                     // 0 = disable , 1 = enable Horizonal mirror the image
        s->set_lenc(s, 1);                        // 0 = disable , 1 = enable  Enables or disables lens correction
        s->set_raw_gma(s, 1);                     // 0 = disable , 1 = enable Enables or disables gamma curve processing
        s->set_saturation(s, 0);                  // -2 to 2 Adjusts the image saturation.
        s->set_special_effect(s, 0);              // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
        s->set_vflip(s, 0);                       // 0 = disable , 1 = enable Vertical Flip the image
        s->set_wb_mode(s, 0);                     // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        s->set_whitebal(s, 1);                    // 0 = disable , 1 = enable Enables or disables automatic white balance (AWB)
        s->set_wpc(s, 1);                         // 0 = disable , 1 = enable Enables or disables white pixel correction.
        s->set_quality(s, 30);                    //Sets the JPEG compression to low quality
        s->set_framesize(s, FRAMESIZE_QVGA);      //Sets the frame size to QVGA 320x240

    ESP_LOGI("Debug", "Printing updated sensor settings:");
    camera.printSensorValues();

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
