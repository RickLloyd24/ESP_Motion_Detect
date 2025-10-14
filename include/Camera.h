//
// Created by Simone on 07/09/22.
//

#ifndef MOTIONDETECT_CAMERA_H
#define MOTIONDETECT_CAMERA_H

#include <esp_camera.h>
#include <esp_log.h>             // Required for ESP-IDF logging macros

// Global variables for frame management
extern camera_fb_t *gFrame;
extern sensor_t *setCamSensor;
extern camera_config_t *setCamConfig;

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
        void config(camera_config_t config);                    // Custom Camera Configuration

        /**
         * Function to get the current camera_fb_t frame
         */
        camera_fb_t* getFramePtr();
        uint8_t* getFrameBufptr();
        size_t getFrameSize();

    protected:
        camera_config_t _config;
        // ChooseModel functions
        void _aithinker(camera_config_t* config);
        void _m5(camera_config_t* config);
        void _m5wide(camera_config_t* config);
        void _eye(camera_config_t* config);
        void _wrover(camera_config_t* config);
    };
}

#endif //MOTIONDETECT_CAMERA_H
