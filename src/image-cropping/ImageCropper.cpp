#include "ImageCropper.hpp"

ImageCropper::ImageCropper(const cv::Scalar markColor, 
        const cv::Scalar cropColor) {
    this->markColor = markColor;
    this->cropColor = cropColor;
}

ImageCropper::ImageCropper() {
    ImageCropper(cv::Scalar(0,0,255), cv::Scalar(0,0,0)); // Red and black
}

void ImageCropper::setImage(cv::Mat &img) {
    image = img;
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

std::vector<cv::Point> ImageCropper::getVehicleContour() {
    return vehicleContourInitialized ? vehicleContour : initVehicleContour();
}

std::vector<cv::Point> ImageCropper::initVehicleContour() {
    cv::Size imageSize = image.size();
    vehicleContour.push_back(cv::Point(0, imageSize.height));
    vehicleContour.push_back(cv::Point(0, 423));
    vehicleContour.push_back(cv::Point(72, 406));
    vehicleContour.push_back(cv::Point(72, 446));
    vehicleContour.push_back(cv::Point(137, 426));
    vehicleContour.push_back(cv::Point(139, 391));
    vehicleContour.push_back(cv::Point(219, 374));
    vehicleContour.push_back(cv::Point(266, 373));
    vehicleContour.push_back(cv::Point(272, 362));
    vehicleContour.push_back(cv::Point(377, 363));
    vehicleContour.push_back(cv::Point(379, 369));
    vehicleContour.push_back(cv::Point(430, 372));
    vehicleContour.push_back(cv::Point(509, 397));
    vehicleContour.push_back(cv::Point(511, 433));
    vehicleContour.push_back(cv::Point(581, 453));
    vehicleContour.push_back(cv::Point(581, 412));
    vehicleContour.push_back(cv::Point(imageSize.width, 427));
    vehicleContour.push_back(cv::Point(imageSize.width, imageSize.height));
    vehicleContourInitialized = true;
    return vehicleContour;
}