#include "ImageTracker.hpp"

using namespace cv;
using namespace std;

ImageTracker::ImageTracker(const String templatePath, const int detectionMethod_, const int minRectArea_, const int maxRectArea_) {
    setTemplateImage(templatePath);
    setMethod(detectionMethod_);
    setMinRectArea(minRectArea_);
    setMaxRectArea(maxRectArea_);
}

void ImageTracker::setTemplateImage(const String templatePath) {
    detectionTemplate = cv::imread(templatePath, 0);
}

void ImageTracker::matchAndNormalize(const Mat &inputImage, Mat &outputImage) {
    cv::matchTemplate(inputImage, detectionTemplate, outputImage, detectionMethod);
    cv::normalize(outputImage, outputImage, 0, 1, NORM_MINMAX, -1, Mat() );

    outputImage.convertTo(outputImage, CV_8UC1);
}

vector<Rect> ImageTracker::detectMatches(const Mat &image, Mat &detections) {
    Mat temp = Mat(detections);

    cv::GaussianBlur(image, detections, cv::Size(3, 3), 2);
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

void ImageTracker::mergeOverlappingRectangles(vector<Rect> &rectangles) {
    cv::groupRectangles(rectangles, 0, 1000);
    recursiveMerge(rectangles);
}

void ImageTracker::recursiveMerge(vector<Rect> &rectangles) {
    if (rectangles.size() < 2) {
        return;
    }
    vector<Rect> mergedRectangles;
    vector<bool> usedRectangles(rectangles.size(), false);
    bool mergedSomething = false;
    for (int i = 0; i < rectangles.size()-1; ++i) {
        if (usedRectangles[i]) {
            continue;
        }
        vector<Rect> overlappingRects;
        Rect unionRects = rectangles[i];
        for (int j = i+1; j < rectangles.size(); j++) {
            Rect tempRect = rectangles[j];
            if ((unionRects & tempRect).area() > 0) {
                unionRects = unionRects | tempRect;
                usedRectangles[j] = true;
                mergedSomething = true;
            }
        }
        mergedRectangles.push_back(unionRects);
        usedRectangles[i] = true;        
    }

    if (!mergedSomething) {
        return;
    }
    rectangles = mergedRectangles;
    recursiveMerge(rectangles);
}

void ImageTracker::filterRectsByArea(vector<Rect> &rectangles, int minArea, int maxArea) {
    // temporary array to store the results
    vector<Rect> workingArray;

    // iterate the input array and add those with enough area to 
    for (Rect rect: rectangles) {
        if (rect.area() >= minArea && rect.area() <= maxArea) {
            workingArray.push_back(rect);
        }
    }

    // return all rectangles in the workingArray 
    rectangles = workingArray;
}

void ImageTracker::filterRectsByDimensions(vector<Rect> &rectangles, const double widthToHeightRatio) {
    vector<Rect> rectanglesOut;
    for (Rect rect: rectangles) {
        bool goodDimensions = rect.width < widthToHeightRatio*rect.height;

        if (goodDimensions) {
            rectanglesOut.push_back(rect);
        }
    }

    rectangles = rectanglesOut;    
}

void ImageTracker::run(const Mat &image, vector<Rect> &rectangles) {
    Mat detections;
    vector<Rect> matches;
    matches = detectMatches(image, detections);
    
    mergeOverlappingRectangles(matches);
    filterRectsByArea(matches, minRectArea, maxRectArea);
    filterRectsByDimensions(matches);

    rectangles = matches;
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

void ImageTracker::setMinRectArea(const int minRectArea_) {
    if (minRectArea_ > 0) {
        minRectArea = minRectArea_;
    } else {
        minRectArea = 0;
    }
}
void ImageTracker::setMaxRectArea(const int maxRectArea_) {
    if (maxRectArea_ > minRectArea) {
        maxRectArea = maxRectArea_;
    } else {
        maxRectArea = 5000;
    }
}
