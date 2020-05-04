#include "ImageTracker.hpp"

using namespace cv;
using namespace std;

ImageTracker::ImageTracker(const String templatePath, const int detectionMethod_) {
    setTemplateImage(templatePath);
    setMethod(detectionMethod_);
}

void ImageTracker::setTemplateImage(const String templatePath) {
    detectionTemplate = cv::imread(templatePath, 0);
}

vector<Rect> ImageTracker::detectMatches(const Mat &image, Mat &detections) {
    cv::matchTemplate(image, detectionTemplate, detections, detectionMethod);
    cv::normalize(detections, detections, 0, 1, NORM_MINMAX, -1, Mat() );
    
    cv::Mat temp = cv::Mat(detections);

    detections.convertTo(detections, CV_8UC1);
    
    // cv::GaussianBlur(image, detections, cv::Size(3, 3), 2);
    vector<vector<Point> > contours;
    findContours( detections, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );

    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>centers( contours.size() );
    
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i], contours_poly[i], 3, true );
        boundRect[i] = boundingRect( contours_poly[i] );
    }

    detections = temp;
    return boundRect;
}


void ImageTracker::findObjectLocation(const Mat &image, Point &bestMatch) {
    cv::Mat detections;
    cv::matchTemplate(image, detectionTemplate, detections, detectionMethod);
    cv::normalize(detections, detections, 0, 1, NORM_MINMAX, -1, Mat() );

    double minVal;      
    double maxVal; 
    cv::Point minLoc;
    cv::Point maxLoc;

    // localize the detection with the highest confidence
    minMaxLoc(detections, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
        
    if (highValuesMatchBest) {
        bestMatch = maxLoc;
    } else {
        bestMatch = minLoc;
    }
}

void ImageTracker::setMethod(const int detectionMethod_) {
    detectionMethod = detectionMethod_; 
    if (detectionMethod == CV_DIFF || detectionMethod == CV_TM_SQDIFF_NORMED) {
        highValuesMatchBest = false;
    } else {
        highValuesMatchBest = true;
    }
}

int ImageTracker::getTemplateWidth() {
    return detectionTemplate.cols;
}

int ImageTracker::getTemplateHeight() {
    return detectionTemplate.rows;
}


