#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

#include <utility>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <iostream>

class ImageTracker {
    public:
        /**the rectangeles
         * Create instance of ImageTracker to detect cones 
         * 
         * @param minRectArea_ default minRectArea for this ImageTracker
         * @param maxRectArea_ default maxRectArea for this ImageTracker 
         */
        ImageTracker(const int minRectArea_=100, const int maxRectArea_=5000);
        
        /**
         * Detect contours of possible objects in an Image
         * 
         * @param image[in] The Image detections are made on
         * @param detections[out] Return the original image
         * @return Vector containing rectangular regions 
         */
        std::vector<cv::Rect> detectMatches(const cv::Mat &image, cv::Mat &detections);

        /**
         * Recursively merge overlapping rectangles until no more rectangles overlap
         * 
         * @param rectangles Rectangles to be merged (in and out)
         */
        void recursiveMerge(std::vector<cv::Rect> &rectangles);
       
        /**
         * Filter out all rectangles based on their area
         * 
         * @param rectangles are the rectangles to be filtered
         * @param minArea is the minimum area a rectangle may be to be considered a cone
         * @param maxArea is the maximum size a rectangle may be to be considered a cone
         */
        void filterRectsByArea(std::vector<cv::Rect> &rectangles, int minArea, int maxArea);

        /**
         * Filter out all rectangles that don't come in the desired widthToHeightRatio
         * 
         * @param rectangles Rectangles that need to be filtered (also used to return the result)
         * @param widthToHeightRatio Maximum ratio width/height that passes the filter (default=1.0)
         */
        void filterRectsByDimensions(std::vector<cv::Rect> &rectangles, const double widthToHeightRatio=1.0);
        
        /**
         * Merge overlapping Rectangles into a single rectangle.
         * 
         * @param rectangles Unmerged rectangles that are merged (and returned in the same vector)
         */
        void mergeOverlappingRectangles(std::vector<cv::Rect> &rectangles);
        
        /**
         * Run the ImageTracker (actually detector) on a given image and return rectangles for detections
         * 
         * @param[in] image Address of the image we want to detect cones on
         * @param[out] rectangles Vector of rectangles marking cones in the image
         */
        void run(const cv::Mat &image, std::vector<cv::Rect> &rectangles);

        /**
         * Set the minimum area that detection rectangles need have to be considered cones
         * 
         * @param minRectArea_ new value to be set for minRectArea
         */
        void setMinRectArea(const int minRectArea_);

        /**
         * Set the maximum area that detection rectangles are allowed to have to be considered cones
         * 
         * @param maxRectArea_ new value to be set for maxRectArea
         */
        void setMaxRectArea(const int maxRectArea_);        

    private:
        int minRectArea = 100; // Min area to be classified as a cone
        int maxRectArea = 5000; // Max area to be classified as a cone
};
#endif