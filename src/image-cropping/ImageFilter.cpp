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

    img =applyBlue1(hsv, blue1)+applyBlue2(hsv, blue2)+applyYellow(hsv, yellow) ;

}

Mat applyBlue1(const Mat img,tuple<Scalar, Scalar> tuple){
    Mat filter;
    inRange(hsv, tuple.first, tuple.second,filter);
    return filter;
}


Mat applyBlue2(const Mat img,tuple<Scalar, Scalar> tuple){
    Mat filter;
    inRange(hsv, tuple.first, tuple.second,filter);
    return filter;
}


Mat applyYellow(const Mat img,tuple<Scalar, Scalar> tuple){
    Mat filter;
    inRange(hsv, tuple.first, tuple.second,filter);
    return filter;
}




