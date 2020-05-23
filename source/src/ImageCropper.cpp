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
    cv::rectangle(image, markRect, cv::Scalar(0,0,255,255));    // Mark rectangle and outline it with red color
}

void ImageCropper::cropRectangle(const cv::Rect &cropRect) {
    cv::rectangle(image, cropRect, cropColor, CV_FILLED);       // Marked rectangle gets cropped and filled using CV_FILLED with the color cropcolor 
}

void ImageCropper::markPolygon(const std::vector<cv::Point> &markContour) {
    // convert vector
    int npts;
    const cv::Point *pts = transformVectorToArray(cropContour, npts);

    // mark polyline
    cv::polylines(image, &pts, &npts, 1, true, markColor);
}

void ImageCropper::cropPolygon(const std::vector<cv::Point> &cropContour) {
    // Convert vector
    int npts;
    const cv::Point *pts = transformVectorToArray(cropContour, npts);

    // fill polygon
    cv::fillPoly(image, &pts, &npts, 1, cropColor);    
}

const cv::Point * transformVectorToArray(const std::vector<cv::Point> &vector, int &nrOfArrayPoints) {
    // convert vector to c-type array (necessary for polylines function)
    nrOfArrayPoints = cv::Mat(vector).rows;
    return (const cv::Point*) cv::Mat(vector).data;
}