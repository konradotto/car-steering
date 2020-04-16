#include "ImageCropper.hpp"

ImageCropper::ImageCropper(cv::Mat &img, const cv::Scalar markColor, 
        const cv::Scalar cropColor) {
    image = img;
    this->markColor = markColor;
    this->cropColor = cropColor;
}

ImageCropper::ImageCropper(cv::Mat &img) {
    ImageCropper(img, cv::Scalar(0,0,255), cv::Scalar(0,0,0)); // Red and black
}

void ImageCropper::markRectangle(const cv::Rect &markRect) {
    cv::rectangle(image, markRect, cropColor);
}

void ImageCropper::cropRectangle(const cv::Rect &cropRect) {
    cv::rectangle(image, cropRect, cropColor, CV_FILLED);
}

void ImageCropper::markPolygon(const std::vector<cv::Point> &markContour) {

}

void ImageCropper::cropPolygon(const std::vector<cv::Point> &cropContour) {
    
}