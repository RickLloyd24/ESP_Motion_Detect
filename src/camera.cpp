//
// Created by Simone on 07/09/22.
// Camera implementation file
//

#include "Camera.h"
#include <Arduino.h>

// Global variable definitions
camera_fb_t *gFrame = nullptr;
sensor_t *setCamSensor = nullptr;
camera_config_t *setCamConfig = nullptr;

namespace MotionDetect {

    /**
     * Constructor
     */
    Camera::Camera() {
        ESP32_Cam();                                        // Default to ESP32-CAM model
        _config.pixel_format = PIXFORMAT_JPEG;
        _config.grab_mode = CAMERA_GRAB_LATEST;
        if(psramFound()){
            _config.frame_size = FRAMESIZE_VGA;
            _config.fb_location = CAMERA_FB_IN_PSRAM;
            _config.jpeg_quality = 10;
            _config.fb_count = 2;   
        }
        else {
            // Limit the frame size when PSRAM is not available
            _config.frame_size = FRAMESIZE_VGA;
            _config.fb_location = CAMERA_FB_IN_DRAM;
            _config.jpeg_quality = 20;
            _config.fb_count = 1;
        }
        esp_log_level_set("camera", ESP_LOG_DEBUG);
    }

    /**
     * Set Camera board to one of the following options
     */
    void Camera::aithinker() { _aithinker(&_config); }
    void Camera::m5() { _m5(&_config); }
    void Camera::m5wide() { _m5wide(&_config); }
    void Camera::eye() { _eye(&_config); }
    void Camera::wrover() { _wrover(&_config); }
    void Camera::ESP32_Cam() { _aithinker(&_config); }

    /**
     * Initialize the camera
     * @return
     */
    bool Camera::begin() {
        if (esp_camera_init(&_config) != ESP_OK) {
            ESP_LOGE("begin", "ESP camera init error");
            return false;
        }
        setCamSensor = esp_camera_sensor_get();
        	setCamSensor->set_ae_level(setCamSensor, 0);       // -2 to 2 Adjusts the auto exposure level
            setCamSensor->set_aec_value(setCamSensor, 168);    // 0 to 1200 Sets the manual exposure value when AEC is disabled
            setCamSensor->set_aec2(setCamSensor, 1);           // 0 = disable , 1 = enable Enables or disables AEC 2, an enhanced AEC algorithm
            setCamSensor->set_agc_gain(setCamSensor, 5);       // 0 to 30  Sets the manual gain value when AGC is disabled.
            setCamSensor->set_awb_gain(setCamSensor, 1);       // 0 = disable , 1 = enable Enables or disables white balance gain control
            setCamSensor->set_bpc(setCamSensor, 0);            // 0 = disable , 1 = enable Enables or disables black pixel correction
            setCamSensor->set_brightness(setCamSensor, 0);    //-2 to 2 Adjusts the image brightness. Values are typically in the range of.
            setCamSensor->set_colorbar(setCamSensor, 0);       // 0 = disable , 1 = enable Enables a test color bar pattern for debugging
            setCamSensor->set_contrast(setCamSensor, 0);       // -2 to 2 Adjusts the image contrast.
            setCamSensor->set_dcw(setCamSensor, 1);            // 0 = disable , 1 = enable  Enables or disables downsize cropping. 
            setCamSensor->set_exposure_ctrl(setCamSensor, 1);  // 0 = disable , 1 = enable Enables or disables automatic exposure control (AEC)
            setCamSensor->set_gain_ctrl(setCamSensor, 1);      // 0 = disable , 1 = enable Enables or disables automatic gain control (AGC).
            setCamSensor->set_gainceiling(setCamSensor, (gainceiling_t)0);  // 0 to 6 Sets the maximum allowed gain when AGC is enabled. This can be used to control noise levels.
            setCamSensor->set_hmirror(setCamSensor, 1);        // 0 = disable , 1 = enable Horizonal mirror the image
            setCamSensor->set_lenc(setCamSensor, 1);           // 0 = disable , 1 = enable  Enables or disables lens correction
            setCamSensor->set_raw_gma(setCamSensor, 1);        // 0 = disable , 1 = enable Enables or disables gamma curve processing
            setCamSensor->set_saturation(setCamSensor, 0);     // -2 to 2 Adjusts the image saturation.
            setCamSensor->set_special_effect(setCamSensor, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
            setCamSensor->set_vflip(setCamSensor, 0);          // 0 = disable , 1 = enable Vertical Flip the image
            setCamSensor->set_wb_mode(setCamSensor, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
            setCamSensor->set_whitebal(setCamSensor, 1);       // 0 = disable , 1 = enable Enables or disables automatic white balance (AWB)
            setCamSensor->set_wpc(setCamSensor, 1);            // 0 = disable , 1 = enable Enables or disables white pixel correction.
        return true;
    }

    /**
     * General purpose getters for information about the current frame
     */
    camera_fb_t* Camera::getFramePtr() { return gIsFrame() ? gFrame : NULL; }
    uint8_t* Camera::getFrameBufptr() { return gIsFrame() ? gFrame->buf : NULL; }
    size_t Camera::getFrameSize() { return gIsFrame() ? gFrame->len : 0; }

    /**
     * Capture new frame
     * @return true if successful, false otherwise
     */
    bool Camera::capture() {
        // release old frame first
        if (gFrame) {
            esp_camera_fb_return(gFrame);
            gFrame = NULL;
        }

        gFrame = esp_camera_fb_get();

        if (!gFrame) {
          ESP_LOGW("Capture", "Capture error");
          return false;
        }
        return true;
    }
    /**
     * Custom Camera Configuration
     * @parm config Camera configuration to apply
     */
    void Camera::config(camera_config_t config) {
        _config = config;
    }
    /**
     * ChooseModel functions
     */
    void Camera::_aithinker(camera_config_t* config) {
        config->pin_d0 = 5;
        config->pin_d1 = 18;
        config->pin_d2 = 19;
        config->pin_d3 = 21;
        config->pin_d4 = 36;
        config->pin_d5 = 39;
        config->pin_d6 = 34;
        config->pin_d7 = 35;
        config->pin_xclk = 0;
        config->pin_pclk = 22;
        config->pin_vsync = 25;
        config->pin_href = 23;
        config->pin_sccb_sda = 26;
        config->pin_sccb_scl = 27;
        config->pin_pwdn = 32;
        config->pin_reset = -1;
    }

    void Camera::_m5(camera_config_t* config) {
        config->pin_d0 = 32;
        config->pin_d1 = 35;
        config->pin_d2 = 34;
        config->pin_d3 = 5;
        config->pin_d4 = 39;
        config->pin_d5 = 18;
        config->pin_d6 = 36;
        config->pin_d7 = 19;
        config->pin_xclk = 27;
        config->pin_pclk = 21;
        config->pin_vsync = 22;
        config->pin_href = 26;
        config->pin_sccb_sda = 25;
        config->pin_sccb_scl = 23;
        config->pin_pwdn = -1;
        config->pin_reset = 15;
    }

    void Camera::_m5wide(camera_config_t* config) {
        config->pin_d0 = 32;
        config->pin_d1 = 35;
        config->pin_d2 = 34;
        config->pin_d3 = 5;
        config->pin_d4 = 39;
        config->pin_d5 = 18;
        config->pin_d6 = 36;
        config->pin_d7 = 19;
        config->pin_xclk = 27;
        config->pin_pclk = 21;
        config->pin_vsync = 25;
        config->pin_href = 26;
        config->pin_sccb_sda = 22;
        config->pin_sccb_scl = 23;
        config->pin_pwdn = -1;
        config->pin_reset = 15;
    }

    void Camera::_eye(camera_config_t* config) {
        config->pin_d0 = 34;
        config->pin_d1 = 13;
        config->pin_d2 = 14;
        config->pin_d3 = 35;
        config->pin_d4 = 39;
        config->pin_d5 = 38;
        config->pin_d6 = 37;
        config->pin_d7 = 36;
        config->pin_xclk = 4;
        config->pin_pclk = 25;
        config->pin_vsync = 5;
        config->pin_href = 27;
        config->pin_sccb_sda = 18;
        config->pin_sccb_scl = 23;
        config->pin_pwdn = -1;
        config->pin_reset = -1;
    }

    void Camera::_wrover(camera_config_t* config) {
        config->pin_d0 = 4;
        config->pin_d1 = 5;
        config->pin_d2 = 18;
        config->pin_d3 = 19;
        config->pin_d4 = 36;
        config->pin_d5 = 39;
        config->pin_d6 = 34;
        config->pin_d7 = 35;
        config->pin_xclk = 21;
        config->pin_pclk = 22;
        config->pin_vsync = 25;
        config->pin_href = 23;
        config->pin_sccb_sda = 26;
        config->pin_sccb_scl = 27;
        config->pin_pwdn = -1;
        config->pin_reset = -1;
        config->xclk_freq_hz = 20000000;
    }

} // namespace MotionDetect
