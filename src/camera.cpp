//
// Created by Simone on 07/09/22.
// Camera implementation file
//

#include "Camera.h"
#include <Arduino.h>

// Global variable definitions
camera_fb_t *gFrame = nullptr;
//sensor_t *setCamSensor = nullptr;
//camera_config_t *setCamConfig = nullptr;

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
     * Runtime camera configuration functions
     * These functions update the camera configuration before initialization
     */
    void Camera::setFrameSize(framesize_t frameSize) {
        _config.frame_size = frameSize;
    }

    void Camera::setJpegQuality(int quality) {
        _config.jpeg_quality = quality;
    }

    void Camera::setFrameBufferLocation(camera_fb_location_t location) {
        _config.fb_location = location;
    }

    void Camera::setFrameBufferCount(size_t count) {
        _config.fb_count = count;
    }

    void Camera::setPixelFormat(pixformat_t format) {
        _config.pixel_format = format;
    }

    void Camera::setGrabMode(camera_grab_mode_t mode) {
        _config.grab_mode = mode;
    }

    void Camera::setXclkFreq(uint32_t freq) {
        _config.xclk_freq_hz = freq;
    }

    /**
     * Helper function to get frame size name from enum
     */
    const char* Camera::_getFrameSizeName(framesize_t frameSize) {
        switch (frameSize) {
            case FRAMESIZE_96X96: return "96x96";
            case FRAMESIZE_QQVGA: return "QQVGA (160x120)";
            case FRAMESIZE_QCIF: return "QCIF (176x144)";
            case FRAMESIZE_HQVGA: return "HQVGA (240x176)";
            case FRAMESIZE_240X240: return "240x240";
            case FRAMESIZE_QVGA: return "QVGA (320x240)";
            case FRAMESIZE_CIF: return "CIF (400x296)";
            case FRAMESIZE_HVGA: return "HVGA (480x320)";
            case FRAMESIZE_VGA: return "VGA (640x480)";
            case FRAMESIZE_SVGA: return "SVGA (800x600)";
            case FRAMESIZE_XGA: return "XGA (1024x768)";
            case FRAMESIZE_HD: return "HD (1280x720)";
            case FRAMESIZE_SXGA: return "SXGA (1280x1024)";
            case FRAMESIZE_UXGA: return "UXGA (1600x1200)";
            case FRAMESIZE_FHD: return "FHD (1920x1080)";
            case FRAMESIZE_P_HD: return "P_HD (720x1280)";
            case FRAMESIZE_P_3MP: return "P_3MP (864x1536)";
            case FRAMESIZE_QXGA: return "QXGA (2048x1536)";
            case FRAMESIZE_QHD: return "QHD (2560x1440)";
            case FRAMESIZE_WQXGA: return "WQXGA (2560x1600)";
            case FRAMESIZE_P_FHD: return "P_FHD (1080x1920)";
            case FRAMESIZE_QSXGA: return "QSXGA (2560x1920)";
            default: return "UNKNOWN";
        }
    }

    /**
     * Helper function to get pixel format name from enum
     */
    const char* Camera::_getPixelFormatName(pixformat_t pixelFormat) {
        switch (pixelFormat) {
            case PIXFORMAT_RGB565: return "RGB565";
            case PIXFORMAT_YUV422: return "YUV422";
            case PIXFORMAT_YUV420: return "YUV420";
            case PIXFORMAT_GRAYSCALE: return "GRAYSCALE";
            case PIXFORMAT_JPEG: return "JPEG";
            case PIXFORMAT_RGB888: return "RGB888";
            case PIXFORMAT_RAW: return "RAW";
            case PIXFORMAT_RGB444: return "RGB444";
            case PIXFORMAT_RGB555: return "RGB555";
            default: return "UNKNOWN";
        }
    }

    /**
     * Helper function to get special effect name from enum
     */
    const char* Camera::_getSpecialEffectName(int effect) {
        switch (effect) {
            case 0: return "No Effect";
            case 1: return "Negative";
            case 2: return "Grayscale";
            case 3: return "Red Tint";
            case 4: return "Green Tint";
            case 5: return "Blue Tint";
            case 6: return "Sepia";
            default: return "Unknown Effect";
        }
    }

    /**
     * Helper function to get white balance mode name from enum
     */
    const char* Camera::_getWhiteBalanceModeName(int mode) {
        switch (mode) {
            case 0: return "Auto";
            case 1: return "Sunny";
            case 2: return "Cloudy";
            case 3: return "Office";
            case 4: return "Home";
            default: return "Unknown Mode";
        }
    }

    /**
     * Print all current sensor configuration values for debugging
     */
    void Camera::printSensorValues() {
        Serial.printf("=== Current Camera Sensor Settings ===");
        // Print camera configuration Information
        Serial.printf("\n--- Camera Configuration ---\n");
        Serial.printf("Pixel Format: %s\n", _getPixelFormatName(_config.pixel_format));
        Serial.printf("Frame Buffers: %d\n", _config.fb_count);
        Serial.printf("XCLK Frequency: %lu Hz\n", _config.xclk_freq_hz);
        const char* fb_location = (_config.fb_location == CAMERA_FB_IN_PSRAM) ? "PSRAM" : "DRAM";
        Serial.printf("Frame Buffer Location: %s\n", fb_location);
        const char* grab_mode = (_config.grab_mode == CAMERA_GRAB_LATEST) ? "LATEST" : "WHEN_EMPTY";
        Serial.printf("Grab Mode: %s\n", grab_mode);

        sensor_t *s = esp_camera_sensor_get();
        Serial.printf("--- Sensor Controls ---\n");
        Serial.printf("Frame Size: %s\n", _getFrameSizeName(s->status.framesize));
        Serial.printf("JPEG Quality: %d (0=highest, 63=lowest)\n", s->status.quality);
        Serial.printf("Brightness: %d (-2 to 2)\n", s->status.brightness);
        Serial.printf("Contrast: %d (-2 to 2)\n", s->status.contrast);
        Serial.printf("Saturation: %d (-2 to 2)\n", s->status.saturation);
        Serial.printf("Sharpness: %d\n", s->status.sharpness);
        Serial.printf("Denoise: %d\n", s->status.denoise);
        Serial.printf("Special Effect: %s\n", _getSpecialEffectName(s->status.special_effect));
        Serial.printf("Scale: %s\n", s->status.scale ? "TRUE" : "FALSE");
        Serial.printf("Binning: %s\n", s->status.binning ? "TRUE" : "FALSE");

        Serial.printf("--- Exposure & Gain ---\n");
        Serial.printf("Auto Exposure: %s\n", s->status.aec ? "ON" : "OFF");
        Serial.printf("AEC Value: %d (0-1200)\n", s->status.aec_value);
        Serial.printf("AE Level: %d (-2 to 2)\n", s->status.ae_level);
        Serial.printf("Auto Gain: %s\n", s->status.agc ? "ON" : "OFF");
        Serial.printf("AGC Gain: %d (0-30)\n", s->status.agc_gain);
        Serial.printf("Gain Ceiling: %d (0-6)\n", s->status.gainceiling);

        Serial.printf("--- White Balance ---\n");
        Serial.printf("Auto WB: %s\n", s->status.awb ? "ON" : "OFF");
        Serial.printf("AWB Gain: %s\n", s->status.awb_gain ? "ON" : "OFF");
        Serial.printf("WB Mode: %s\n", _getWhiteBalanceModeName(s->status.wb_mode));

        Serial.printf("--- Image Corrections ---\n");
        Serial.printf("H-Mirror: %s\n", s->status.hmirror ? "ON" : "OFF");
        Serial.printf("V-Flip: %s\n", s->status.vflip ? "ON" : "OFF");
        Serial.printf("Lens Correction: %s\n", s->status.lenc ? "ON" : "OFF");
        Serial.printf("DCW (Downsize): %s\n", s->status.dcw ? "ON" : "OFF");
        Serial.printf("BPC (Black Pixel): %s\n", s->status.bpc ? "ON" : "OFF");
        Serial.printf("WPC (White Pixel): %s\n", s->status.wpc ? "ON" : "OFF");
        Serial.printf("Raw GMA: %s\n", s->status.raw_gma ? "ON" : "OFF");

        Serial.printf("--- Test Patterns ---\n");
        Serial.printf("Color Bar: %s\n", s->status.colorbar ? "ON" : "OFF");
        
        Serial.printf("=======================================\n");
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

// Global camera instance definition
//MotionDetect::Camera camera;