#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

#include <utility>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

class ImageTracker {
    public:
        ImageTracker(const String templatePath, const int detectionMethod_);
        void setTemplateImage(const String templatePath);
        vector<Rect> detectMatches(const Mat &image, Mat &detections);
        void findObjectLocation(const Mat &image, Point &bestMatch);
        void setMethod(const int detectionMethod_);
        int getTemplateWidth();
        int getTemplateHeight();

    private:
        cv::Mat detectionTemplate;
        int detectionMethod;
        bool highValuesMatchBest;
};
#endif