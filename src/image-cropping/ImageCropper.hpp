#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include <opencv2/imgproc/imgproc.hpp>

class ImageCropper {
    public:
        ImageCropper(const cv::Scalar markColor, const cv::Scalar cropColor);
        ImageCropper();
        void setImage(cv::Mat &img);
        void markRectangle(const cv::Rect &markRect);
        void cropRectangle(const cv::Rect &cropRect);
        void markPolygon(const std::vector<cv::Point> &markContour);
        void cropPolygon(const std::vector<cv::Point> &cropContour);
        std::vector<cv::Point> getVehicleContour() { return vehicleContour; };
    private:
        cv::Mat image;
        cv::Scalar markColor;
        cv::Scalar cropColor;
        std::vector<cv::Point> vehicleContour;
        std::vector<cv::Point> initVehicleContour();
        bool vehicleContourInitialized;
};
#endif