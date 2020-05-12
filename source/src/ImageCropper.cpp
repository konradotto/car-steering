#include "ImageCropper.hpp"

ImageCropper::ImageCropper(const cv::Scalar markColor_, 
        const cv::Scalar cropColor_) {
    ImageCropper();
    markColor = markColor_;
    cropColor = cropColor_;
}

void ImageCropper::setImage(cv::Mat &img) {
    image = img;
}

void ImageCropper::markRectangle(const cv::Rect &markRect) {
    cv::rectangle(image, markRect, cv::Scalar(0,0,255,255));
}

void ImageCropper::cropRectangle(const cv::Rect &cropRect) {
    cv::rectangle(image, cropRect, cropColor, CV_FILLED);
}

void ImageCropper::markPolygon(const std::vector<cv::Point> &markContour) {
    const cv::Point *pts = (const cv::Point*) cv::Mat(markContour).data;
    int npts = cv::Mat(markContour).rows;

    cv::polylines(image, &pts, &npts, 1, true, markColor);
}

void ImageCropper::cropPolygon(const std::vector<cv::Point> &cropContour) {
    const cv::Point *pts = (const cv::Point*) cv::Mat(cropContour).data;
    int npts = cv::Mat(cropContour).rows;

    cv::fillPoly(image, &pts, &npts, 1, cropColor);    
}