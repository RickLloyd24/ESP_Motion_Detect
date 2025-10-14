//
// Created by Simone on 08/09/22.
// Motion detection implementation file
//

#include "Motion.h"

#define minImageSize 96*96   // Minimum image size to process

uint16_t gOffset = 0;
uint8_t* gJpegBuf = nullptr;
size_t gJpegSize = 0;

/**
 * JPEG decoder callback function (must be a free function for C callback)
 * @param dest - destination buffer
 * @param chunkSize - size of the chunk to read
 * @param read - pointer to the variable to store the number of bytes read
 * @param data - user data (not used)
 * @return 0 if successful, error code otherwise
 */
unsigned char jpeg_decode_callback(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data)
{
    if (gJpegSize == 0 || gJpegBuf == nullptr) {
        *read = 0;
        return 0;
    }

    uint16_t n = gJpegSize - gOffset > chunkSize ? chunkSize : gJpegSize - gOffset;

    if (gOffset > gJpegSize)
        return PJPG_STREAM_READ_ERROR;

    memcpy(dest, gJpegBuf + gOffset, n);
    *read = n;
    gOffset += n;

    return 0;
}

namespace MotionDetect {

    /**
     * Constructor
     */
    Motion::Motion() : pixelCntArray(nullptr), pixelArrayIndex(0), numPixelChanges(0), 
                      percentPixelDiff(0.0), minPercentPixelDiff(20.0), minPixelChange(10),
                      imageSize(0), pixelArraySize(0), BufSizeChange(0.0), minBufSizeChange(20.0), 
                      motionCount(1) {
        esp_log_level_set("motion", ESP_LOG_DEBUG);
    }
    
    /**
     * Destructor - clean up allocated memory
     */
    Motion::~Motion() {
        if (pixelCntArray != nullptr) {
            free(pixelCntArray);
            pixelCntArray = nullptr;
        }
    }

    // Configuration methods
    void Motion::setMinPixelDiff(float value) { 
        if (value < 0.0 || value > 100.0) {
            ESP_LOGW("setMinPixelDiff", "Invalid minPercentPixelDiff value, must be between 0 and 100, set to 20.0");
            minPercentPixelDiff = 20.0; // set to default
            return;
        }
        minPercentPixelDiff = value; 
    }
    void Motion::setMinChanges(int value) { 
        if (value < 0 || value > 255) {
            ESP_LOGW("setMinChanges", "Invalid minPixelChange value, must be greater than 0 and less than 255, set to 10");
            minPixelChange = 10; // set to default
            return;
        }
        minPixelChange = value; 
    }
    void Motion::setMinBufSizeChanges(float value) {
        if( value < 0.0 || value > 100.0) {
            ESP_LOGW("setMinBufSizeChanges", "Invalid minBufSizeChange value, must be between 0 and 100, set to 10.0");
            minBufSizeChange = 10.0; // set to default
            return;
        }
        minBufSizeChange = value; 
    }
    void Motion::setMotionCount(int value) {
        if (value != 1 && value != 2) {
            ESP_LOGW("setMotionCount", "Invalid motionCount value, must be 1 or 2, set to 2");
            motionCount = 2; // set to both methods detected
            return;
        }
        motionCount = value;
    }
    // Information getters
    float Motion::getPercentDiff() {return percentPixelDiff; }
    float Motion::getBufSizeChange() { return BufSizeChange; }
    uint16_t Motion::getWidth() { return _image.m_width; }
    uint16_t Motion::getHeight() { return _image.m_height; }

    /**
     * Detect motion function
     * @param buf - pointer to the JPEG image buffer
     * @param bufSize - size of the JPEG image buffer
     * @return 0 if error or no motion detected, 
     *         1 if only buf size change above threshold, 
     *         2 if only pixel motion change above threshold, 
     *         3 if both methods detected motion
     */
    int Motion::detect(uint8_t * buf, size_t bufSize) {
        int status = 0; int flag = 0;
        static unsigned long lastImageSize = 0;
        percentPixelDiff = 0.0;
        gJpegBuf = buf;
        gJpegSize = bufSize;

        if (!gJpegBuf || gJpegSize == 0) {
            ESP_LOGE("detect", "Buffer not valid");
            return 0;
        }
    /* Check for buffer size changed more than threshold */
        if (lastImageSize != 0) {
            BufSizeChange = absdiff((float)gJpegSize, (float)lastImageSize) / (float)lastImageSize * 100.0;
            lastImageSize = gJpegSize;
            if (BufSizeChange > minBufSizeChange) {
                bitSet(flag, 0);   //set buf size change detected
                if (motionCount == 1) return flag; // Only frame size change detected
            }
        } else {
            lastImageSize = gJpegSize;
        }

        gOffset = 0;
        // decode jpeg
        if (pjpeg_decode_init(&_image, jpeg_decode_callback, NULL, 1)) {
            ESP_LOGE("Motion","JPEG init error");
            return 0;
        }
        if (imageSize != (_image.m_width * _image.m_height)) {
            imageSize = _image.m_width * _image.m_height;
            if (imageSize < minImageSize) {
                ESP_LOGE("Size", "Image size is less than minimum of %d ", minImageSize);
                return 0;
            }
            allocatePixelArrayBuf(imageSize / 64); // Allocate or reallocate pixel buffer
        }
        pixelArrayIndex = 0;  // Reset pixel array index for each frame
        numPixelChanges = 0;  // Reset pixel change counter for each frame
        while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
            switch (_image.m_scanType) {
                case PJPG_GRAYSCALE:
                    detectPixChg(_image.m_pMCUBufR[0]);
                    break;
                case PJPG_YH1V1:
                    detectPixChg(_image.m_pMCUBufR[0]);
                    break;
                case PJPG_YH1V2:
                    detectPixChg(_image.m_pMCUBufR[0]);
                    detectPixChg(_image.m_pMCUBufR[128]);
                    break;
                case PJPG_YH2V1:
                    detectPixChg(_image.m_pMCUBufR[0]);
                    detectPixChg(_image.m_pMCUBufR[64]);
                    break;
                case PJPG_YH2V2:
                    detectPixChg(_image.m_pMCUBufR[0]);
                    detectPixChg(_image.m_pMCUBufR[64]);
                    detectPixChg(_image.m_pMCUBufR[128]);
                    detectPixChg(_image.m_pMCUBufR[192]);
                    break;
            }
        }
        percentPixelDiff = (float)numPixelChanges / (float)(pixelArrayIndex) * 100.0;
        //Serial.printf("Changes: %04d, Percent: %f, Motion detected: %d \n",  numPixelChanges, percentPixelDiff, percentPixelDiff > minPercentPixelDiff);
        if (percentPixelDiff > minPercentPixelDiff) {
            bitSet(flag, 1);   //set percent pixels change detected;
            return flag;
        }
        return flag;
    }

    /**
     * Allocate or reallocate the pixel buffer
     * @param numBytes - number of bytes to allocate for the buffer
     */
    void Motion::allocatePixelArrayBuf(size_t numBytes) {
        // Free existing buffer if already allocated
        if (pixelCntArray != nullptr) {
            free(pixelCntArray);
            pixelCntArray = nullptr;
            pixelArraySize = 0;
        }
        
        // Allocate new buffer
        if (numBytes > 0) {
            pixelCntArray = (uint8_t*)malloc(numBytes);
            if (pixelCntArray != nullptr) {
                memset(pixelCntArray, 0, numBytes); // Initialize to zero
                pixelArraySize = numBytes;
                ESP_LOGI("Motion", "Allocated pixel buffer: %d bytes", numBytes);
            } else {
                ESP_LOGE("Motion", "Failed to allocate pixel buffer: %d bytes", numBytes);
                pixelArraySize = 0;
            }
        }
    }

    /**
     * Detect pixel changes from previous frame to current frame
     * when pixel change is above threshold, increment numPixelChanges
     * @param p - number of pixels in current block
     */
    void Motion::detectPixChg(uint8_t p) {
        // Bounds checking to prevent heap corruption
        if (pixelCntArray == nullptr || pixelArrayIndex >= pixelArraySize) {
            return;
        }
        
        if(absdiff(p, pixelCntArray[pixelArrayIndex]) > minPixelChange) numPixelChanges++;
        pixelCntArray[pixelArrayIndex] = p;
        pixelArrayIndex++;
    }

} // namespace MotionDetect