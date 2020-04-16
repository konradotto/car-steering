#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include <opencv2/imgproc/imgproc.hpp>

class ImageCropper {
    public:
        ImageCropper(cv::Mat &img);
        void markRectangle(const cv::Rect &markRect);
        void cropRectangle(const cv::Rect &cropRect);
        void markPolygon(const std::vector<cv::Point> &markContour);
        void cropPolygon(const std::vector<cv::Point> &cropContour);
    private:
        cv::Mat internalImage;
};
#endif