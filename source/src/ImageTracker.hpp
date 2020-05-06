#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

#include <utility>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class ImageTracker {
    public:
        ImageTracker(const String templatePath, const int detectionMethod_, const int minRectArea_=100, const int maxRectArea_=5000);
        void setTemplateImage(const String templatePath);
        void matchAndNormalize(const Mat& inputImage, Mat &outputImage);
        vector<Rect> detectMatches(const Mat &image, Mat &detections);
        void findObjectLocation(const Mat &image, Point &bestMatch);
        void recursiveMerge(vector<Rect> &rectangles);
        void filterRectsByArea(vector<Rect> &rectangles, int minArea, int maxArea);
        void filterRectsByDimensions(vector<Rect> &rectangles, const double widthToHeightRatio=1.0);
        void mergeOverlappingRectangles(vector<Rect> &rectangles);
        void run(const Mat &image, vector<Rect> &rectangles);
        void setMethod(const int detectionMethod_);
        int getTemplateWidth();
        int getTemplateHeight();
        void setMinRectArea(const int minRectArea_);
        void setMaxRectArea(const int maxRectArea_);
        

    private:
        cv::Mat detectionTemplate;
        int detectionMethod;
        int minRectArea;
        int maxRectArea;
        bool highValuesMatchBest;
};
#endif