//
// Created by Simone on 08/09/22.
//

#ifndef MOTIONDETECT_MOTION_H
#define MOTIONDETECT_MOTION_H

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picojpeg.h"
#include <esp_log.h>             // Required for ESP-IDF logging macros


// Utility macros
#define absdiff(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

/**
 * JPEG decoder callback function (must be a free function for C callback)
 * @param dest
 * @param chunkSize
 * @param read
 * @param data
 * @return
 */
unsigned char jpeg_decode_callback(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data);

namespace MotionDetect {
    /**
     * Motion detection from JPEG image
     */
    class Motion {
    public:
        /**
         * Constructor
         */
        Motion();
        
        /**
         * Destructor - clean up allocated memory
         */
        ~Motion();

        // Configuration settings
        void setMinPixelDiff(float value);                //set minimum percent of pixels that must change to trigger motion
        void setMinChanges(int value);                    //set minimum number of pixels that must change to count as motion  
        void setMinBufSizeChanges(float value);         //set minimum number of pixels that must change to count as motion  
        void setMotionCount(int value);                   //value 1 = first motion, 2 = both motion methods

        // Information getters
        float getPercentDiff();                           //return last percent difference calculated
        float getBufSizeChange();                       //return last frame size change calculated
        uint16_t getWidth();                              //return image width calculated by the JPEG decoder function
        uint16_t getHeight();                             //return image height calculated by the JPEG decoder function

        int detect(uint8_t * buf, size_t bufSize);        //return 0 if error, 1 if motion detected, 2 if both motion methods detected

    protected:
        uint16_t pixelArrayIndex;
        uint16_t numPixelChanges;
        float percentPixelDiff;
        float minPercentPixelDiff;
        uint16_t minPixelChange;
        pjpeg_image_info_t _image;
        uint8_t* pixelCntArray;            // Dynamic pixel count array buffer
        size_t pixelArraySize;             // Size of allocated buffer
        unsigned long imageSize;
        float minBufSizeChange = 10.0;
        float BufSizeChange = 0.0;
        int motionCount = 1;

        /**
         * Allocate or reallocate the pixel buffer
         * @param numBytes - number of bytes to allocate for the buffer
         */
        void allocatePixelArrayBuf(size_t numBytes);

        /**
         * Detect pixel changes from previous frame to current frame
         * when pixel change is above threshold, increment numPixelChanges
         * @param p - number of pixels in current block
         */
        void detectPixChg(uint8_t p);

    };   
}

#endif //MOTIONDETECT_MOTION_H
