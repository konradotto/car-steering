#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this once per test-runner!

#include <opencv2/imgproc/imgproc.hpp>
#include "catch.hpp"
#include "ImageCropper.hpp"

using namespace cv;
using namespace std;

void cropImage(Mat &img);
double blackRatio(const Mat &img);
vector<Point> initVehicleContour();
Rect aboveHorizon();
Mat creatWhiteimage();

TEST_CASE("Test ImageCropper"){
    Mat img = creatWhiteimage();
    SECTION("Before cropping") {
        double expected = 0.0;
        REQUIRE(blackRatio(img) <= expected);
    }
    SECTION("After cropping"){
        double expected = 0.69;
        cropImage(img);
        REQUIRE(blackRatio(img) >= expected);
    }
}

void cropImage(Mat &img){
    ImageCropper imageCropper = ImageCropper();
    imageCropper.setImage(img);
    imageCropper.cropRectangle(aboveHorizon());
    imageCropper.cropPolygon(initVehicleContour());
}


double blackRatio(const Mat &img) {
    int imgSize = img.rows * img.cols;
    int nonzeros = cv::countNonZero(img);
    double ratio = (imgSize - nonzeros) / double(imgSize);
    return ratio;
}

Mat creatWhiteimage(){
    Mat img = cv::Mat::zeros(Size(640,480), CV_8UC1);      
    img = cv::Scalar(255,255,255);
    return img;
}

Rect aboveHorizon(){
    return Rect(0, 0, 640, (int) (0.52 * 480));
}

vector<Point> initVehicleContour() {
    vector<Point> vehicleContour;
    int width = 640;
    int height = 480;
    vehicleContour.push_back(Point(0, height));
    vehicleContour.push_back(Point(0, 423));
    vehicleContour.push_back(Point(72, 406));
    vehicleContour.push_back(Point(72, 446));
    vehicleContour.push_back(Point(137, 426));
    vehicleContour.push_back(Point(139, 391));
    vehicleContour.push_back(Point(219, 374));
    vehicleContour.push_back(Point(266, 373));
    vehicleContour.push_back(Point(272, 362));
    vehicleContour.push_back(Point(377, 363));
    vehicleContour.push_back(Point(379, 369));
    vehicleContour.push_back(Point(430, 372));
    vehicleContour.push_back(Point(509, 397));
    vehicleContour.push_back(Point(511, 433));
    vehicleContour.push_back(Point(581, 453));
    vehicleContour.push_back(Point(581, 412));
    vehicleContour.push_back(Point(width, 427));
    vehicleContour.push_back(Point(width, height));
    return vehicleContour;
}
