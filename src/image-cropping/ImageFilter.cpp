#include "ImageFilter.hpp"
using namespace cv;
using namespace std;
ImageFilter::ImageFilter() {
    blue1.first=Scalar(100,150,0);
    blue1.second=Scalar(140,255,255);
    blue2.first=Scalar(16, 0, 69);
    blue2.second=Scalar(30, 255, 255);
    yellow.first=Scalar(102, 117, 35);
    yellow.second=Scalar(145, 255, 255);
}

void ImageFilter::applyFilters(Mat &img) {
    Mat hsv, imgYellow, imgBlue;

    cvtColor(img, hsv, COLOR_BGR2HSV);
    imgYellow = applyColorFilter(hsv, yellow);
    imgBlue = applyColorFilter(hsv,blue1) + applyColorFilter(hsv,blue2);
    img = imgYellow + imgBlue;
}

Mat ImageFilter::filterColorRange(Mat &img, vector<pair<Scalar, Scalar>> colorRanges) {
    // convert img to hsv-color-spectrum
    Mat hsv, filteredImage;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    // iterate over color ranges
    bool firstIteration = true;
    for (auto const& colorRange: colorRanges) {
        if (firstIteration) {
            filteredImage = applyColorFilter(hsv, colorRange);
            firstIteration = false;
        } else {
            filteredImage += applyColorFilter(hsv, colorRange);
        }
    }
    return filteredImage;
}

Mat ImageFilter::applyColorFilter(const Mat img, pair<Scalar, Scalar> pair){
    Mat mask;
    Mat filtered(img);
    inRange(img, pair.first, pair.second, mask);
    bitwise_and(filtered, filtered, mask= mask);
    
    return filtered;
}