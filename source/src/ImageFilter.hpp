#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <opencv2/imgproc/imgproc.hpp>

class ImageFilter {

public:

    /**
     * FilterColorRange filters the image based on HSV color range values
     * 
     * @param img is the image being worked on
     * @param colorRanges are the HSV vector pairs we have identified for the various colors needed to filter the cones
     */
    static cv::Mat filterColorRange(const cv::Mat &img, std::vector<std::pair<cv::Scalar, cv::Scalar>> colorRanges);

    /**
     * Detect edges in the passed image using Canny edge detection
     */
    static cv::Mat filterEdges(const cv::Mat &img);
    
    // color ranges for filtering
    static const std::pair <cv::Scalar,cv::Scalar> blue1; 	
    static const std::pair <cv::Scalar,cv::Scalar> blue2;
    static const std::pair <cv::Scalar,cv::Scalar> yellow;
    static const std::pair <cv::Scalar,cv::Scalar> orange;

    // summary of color ranges for each color
    static const std::vector<std::pair<cv::Scalar, cv::Scalar>> blueRanges;
    static const std::vector<std::pair<cv::Scalar, cv::Scalar>> yellowRanges;
    static const std::vector<std::pair<cv::Scalar, cv::Scalar>> orangeRanges;

private:
    
    /**
     * applyColorFilter is the method where the filter is being applied 
     * @param img is the image the filter will be applied on
     * @param pair is the HSV upper and lower ranges that is added to filter the cones.
     */
    static cv::Mat applyColorFilter(const cv::Mat img, std::pair<cv::Scalar, cv::Scalar> pair);
};

#endif