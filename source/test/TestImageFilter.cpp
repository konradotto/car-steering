#include <opencv2/imgproc/imgproc.hpp>
#include "../assets/catch.hpp"
#include "../src/ImageFilter.hpp"

using namespace cv;
using namespace std;

Mat filterYellow(Mat &img);
Mat filterBlue(Mat &img);
double blackRatio(const Mat &img);
Mat creatBlackImage();


TEST_CASE("Test ImageFilter 0."){
    Mat img = creatBlackImage();
    double expected = 1.0;
    SECTION("Before filtering") {
        REQUIRE(blackRatio(filterBlue(img)) >= expected);
    }
    SECTION("Apply blue scale") {
        img = Scalar(240,100,100);
        expected = 0.0;
        REQUIRE(blackRatio(filterBlue(img)) <= expected);
    }
}

TEST_CASE("Test ImageFilter 1."){
    Mat img = creatBlackImage();
    double expected = 1.0;
    SECTION("Before filtering") {
        REQUIRE(blackRatio(filterYellow(img)) >= expected);
    }
    SECTION("Apply yelow scale") {
        img = Scalar(60,100,100);
        expected = 0.0;
        REQUIRE(blackRatio(filterYellow(img)) <= expected);
    }
}

Mat creatBlackImage(){
    Mat img = Mat::zeros(Size(640,480), CV_8UC3);      
    img = Scalar(0,0,0);
    return img;
}

double blackRatio(const Mat &img) {
    int imgSize = img.rows * img.cols;
    int nonzeros = countNonZero(img);
    double ratio = (imgSize - nonzeros) / double(imgSize);
    return ratio;
}

Mat filterYellow(Mat &img){
     return ImageFilter::filterColorRange(img, ImageFilter::yellowRanges);
}

Mat filterBlue(Mat &img){
     return ImageFilter::filterColorRange(img, ImageFilter::blueRanges  );
}

