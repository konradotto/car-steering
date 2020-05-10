#ifndef IMAGE_SAVER_H
#define IMAGE_SAVER_H

#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "ImageFilter.hpp"
#include "ImageCropper.hpp"
#include "ImageTracker.hpp"

class ImageSaver {
    public:
        static void run(cv::Mat const& image, cv::Rect const& aboveHorizon, const std::vector<cv::Point> &vehicleContour);
    private:
        static const std::string dir;
        static const std::string org;
        static const std::string mark;
        static const std::string crop;
        static const std::string blue;
        static const std::string yellow;
        static const std::string filtered;
        static const std::string edges;
        static const std::string matches;
        static const std::string base;
        static const std::string merged;
        static const std::string processed;
        static const std::string type;

        static void drawRects(const std::vector<cv::Rect> &rects, cv::Mat &img, const cv::Scalar &color);
};

#endif