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
    Mat hsv;

    cvtColor(img, hsv, COLOR_BGR2HSV);
    
    img =applyBlue1(hsv, blue1)+applyBlue2(hsv, blue2)+applyYellow(hsv, yellow);
}

Mat ImageFilter::applyBlue1(const Mat img,pair<Scalar, Scalar> pair){
    Mat filter;
    inRange(img, pair.first, pair.second,filter);
    return filter;
}


Mat ImageFilter::applyBlue2(const Mat img,pair<Scalar, Scalar> pair){
    Mat filter;
    inRange(img, pair.first, pair.second,filter);
    return filter;
}


Mat ImageFilter::applyYellow(const Mat img,pair<Scalar, Scalar> pair){
    Mat filter;
    inRange(img, pair.first, pair.second,filter);
    return filter;
}




