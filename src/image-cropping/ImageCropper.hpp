#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include <opencv2/imgproc/imgproc.hpp>

class ImageCropper {
    public:
        ImageCropper(cv::Mat &img);
        cv::Mat markCropAreas();
        cv::Mat cropCropAreas();
    private:
        cv::Mat internalImage;
        void markAboveHorizon();
        void cropAboveHorizon();
        void markCarOutlines();
        void cropCarOutlines();
};
#endif