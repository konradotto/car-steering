#include "ImageCropper.hpp"

ImageCropper::ImageCropper(cv::Mat &img) {
    internalImage = img;
}

cv::Mat ImageCropper::markCropAreas() {
    this->markAboveHorizon();
    this->markCarOutlines();
    return internalImage;
}

cv::Mat ImageCropper::cropCropAreas() {
    this->cropAboveHorizon();
    this->cropCarOutlines();
    return internalImage;
}

void ImageCropper::markAboveHorizon() {

}

void ImageCropper::cropAboveHorizon() {
    
}

void ImageCropper::markCarOutlines() {

}

void ImageCropper::cropCarOutlines() {
    
}