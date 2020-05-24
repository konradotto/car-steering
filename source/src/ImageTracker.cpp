#include "ImageTracker.hpp"

using namespace cv;
using namespace std;

ImageTracker::ImageTracker(const int minRectArea_, const int maxRectArea_) {
    setMinRectArea(minRectArea_); // Set the minimum area of a cone
    setMaxRectArea(maxRectArea_); // Set the maximum area of a cone
}

vector<Rect> ImageTracker::detectMatches(const Mat &image, Mat &detections) {
    Mat temp = Mat(detections);

    cv::GaussianBlur(image, detections, cv::Size(3, 3), 2);// smoothen the image using gaussian blur to improve detection perfomance
    vector<vector<Point> > contours;
   
    findContours( detections, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );//Canny the image
    // CV_CHAIN_APPROX_SIMPLE compress the segments of a image, leaving only endpoints. 
    // A triangle for instance will be compressed until 3 points are left
    // A rectangle compressed until 4 points are left 

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
    cv::groupRectangles(rectangles, 0, 1000); //group all rectangles
    recursiveMerge(rectangles);//Send the groupings to recursiveMerge
}

void ImageTracker::recursiveMerge(vector<Rect> &rectangles) {
    //If numbers of rectangle for a cone is smaller than 2, nothing needs to be merged
    if (rectangles.size() < 2) {
        return;
    }
    
    vector<Rect> mergedRectangles;// temporary array to store the merged rectangles
    vector<bool> usedRectangles(rectangles.size(), false);
    bool mergedSomething = false;
    for (size_t i = 0; i < rectangles.size()-1; ++i) {
        if (usedRectangles[i]) {
            continue;
        }
        vector<Rect> overlappingRects;
        Rect unionRects = rectangles[i];
        for (size_t j = i+1; j < rectangles.size(); j++) {
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

    for (Rect rect: rectangles) {//Iterate the rectangles
        if (rect.area() >= minArea && rect.area() <= maxArea) {
            workingArray.push_back(rect);//if rectangles are within the constraints push them
        }
    }

    // return all rectangles in the workingArray 
    rectangles = workingArray;
}

void ImageTracker::filterRectsByDimensions(vector<Rect> &rectangles, const double widthToHeightRatio) {
    // temporary array to store the rectangles
    vector<Rect> rectanglesOut; 
    
    for (Rect rect: rectangles) {//Iterate the rectangles
        bool goodDimensions = rect.width < widthToHeightRatio*rect.height;//The width is smaller than the rectangle height

        if (goodDimensions) {//If true
            rectanglesOut.push_back(rect);//push the rectangles, otherwise dont register them 
        }
    }
    //Return all rectangles from the array
    rectangles = rectanglesOut;    
}

void ImageTracker::run(const Mat &image, vector<Rect> &rectangles) {
    Mat detections;
    vector<Rect> matches; // temporary array to store the rectangles
    matches = detectMatches(image, detections); //Fill array with rectangles that are detected
    
    mergeOverlappingRectangles(matches); //Send rectangles to merge overlapping rectangles
    filterRectsByArea(matches, minRectArea, maxRectArea);//Send rectangles to filter out by area
    filterRectsByDimensions(matches);//Send rectangles to filter by dimensions

    rectangles = matches;//Return the rectangles from the array
}

void ImageTracker::setMinRectArea(const int minRectArea_) {
    if (minRectArea_ > 0) { //If the area of the rectangle is greater than 0, set it as the area
        minRectArea = minRectArea_;
    } else {
        minRectArea = 0;// if area is 0, rectangle do not exist
    }
}

void ImageTracker::setMaxRectArea(const int maxRectArea_) {
    if (maxRectArea_ > minRectArea) { //If max area of the rectangle is greater than the minimum area
        maxRectArea = maxRectArea_;//Set max area to be max area
    } else {
        maxRectArea = 5000;//Otherwise, default to an area of 5000
    }
}