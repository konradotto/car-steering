#include "ImageCropper.hpp"

ImageCropper::ImageCropper(cv::Mat &img) {
    internalImage = img;
}

void ImageCropper::markRectangle(const cv::Rect &markRect) {

}

void ImageCropper::cropRectangle(const cv::Rect &cropRect) {
    
}

void ImageCropper::markPolygon(const std::vector<cv::Point> &markContour) {

}

void ImageCropper::cropPolygon(const std::vector<cv::Point> &cropContour) {
    
}