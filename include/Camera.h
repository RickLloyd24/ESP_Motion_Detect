//
// Created by Simone on 07/09/22.
//

#ifndef MOTIONDETECT_CAMERA_H
#define MOTIONDETECT_CAMERA_H

#include <esp_camera.h>
#include <esp_log.h>             // Required for ESP-IDF logging macros

// Global variables for frame management
extern camera_fb_t *gFrame;
//extern sensor_t *setCamSensor;
//extern camera_config_t *setCamConfig;

// Macro for frame checking
#define gIsFrame() (gFrame != NULL && gFrame->len > 0)


namespace MotionDetect {
    /**
     * Esp32 cam interface
     */
    class Camera {
    public:
        /**
         * Constructor
         */
        Camera();

        /**
         * Set Camera board to one of the following options
         */
        void aithinker();
        void m5();
        void m5wide();
        void eye();
        void wrover();
        void ESP32_Cam();

        /**
         * Camera control functions
         * @return true if successful, false otherwise
         */       
        bool begin();                                           // Initialize the camera
        bool capture();                                         // Capture new frame
        void config(camera_config_t config);                   // Custom Camera Configuration

        /**
         * Runtime camera configuration functions
         * These functions update the camera configuration before initialization
         */
        void setFrameSize(framesize_t frameSize);               // Set frame size (FRAMESIZE_QVGA, FRAMESIZE_VGA, etc.)
        void setJpegQuality(int quality);                       // Set JPEG quality (0-63, lower = higher quality)
        void setFrameBufferLocation(camera_fb_location_t location); // Set frame buffer location (PSRAM/DRAM)
        void setFrameBufferCount(size_t count);                 // Set number of frame buffers
        void setPixelFormat(pixformat_t format);                // Set pixel format (JPEG, RGB565, etc.)
        void setGrabMode(camera_grab_mode_t mode);              // Set grab mode (LATEST, WHEN_EMPTY)
        void setXclkFreq(uint32_t freq);                        // Set XCLK frequency

        /**
         * Function to get the current camera_fb_t frame
         */
        camera_fb_t* getFramePtr();
        uint8_t* getFrameBufptr();
        size_t getFrameSize();

        /**
         * Debug and monitoring functions
         */
        void printSensorValues();                               // Print all current sensor settings

    protected:
        camera_config_t _config;
        // ChooseModel functions
        void _aithinker(camera_config_t* config);
        void _m5(camera_config_t* config);
        void _m5wide(camera_config_t* config);
        void _eye(camera_config_t* config);
        void _wrover(camera_config_t* config);
        // Helper functions for enum to string conversion
        const char* _getFrameSizeName(framesize_t frameSize);
        const char* _getPixelFormatName(pixformat_t pixelFormat);
        const char* _getSpecialEffectName(int effect);
        const char* _getWhiteBalanceModeName(int mode);
        sensor_t *setCamSensor = nullptr;
    };
}

// Global camera instance
extern MotionDetect::Camera camera;

#endif //MOTIONDETECT_CAMERA_H
