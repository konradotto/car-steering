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
    const cv::Point *pts = (const cv::Point*) cv::Mat(markContour).data;
    int npts = cv::Mat(markContour).rows;

    cv::polylines(image, &pts, &npts, 1, true, cropColor);
}

void ImageCropper::cropPolygon(const std::vector<cv::Point> &cropContour) {
    const cv::Point *pts = (const cv::Point*) cv::Mat(cropContour).data;
    int npts = cv::Mat(cropContour).rows;

    cv::fillPoly(image, &pts, &npts, 1, cropColor);    
}