#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this once per test-runner!

#include <opencv2/imgproc/imgproc.hpp>
#include "../assets/catch.hpp"
#include "../src/ImageCropper.hpp"

using namespace cv;
using namespace std;

void cropImage(Mat &img);
double blackRatio(const Mat &img);
vector<Point> initVehicleContour();
Rect aboveHorizon();
Mat creatWhiteImage();
Mat creatWhitePolyImage();
Mat combineImages(const Mat &img1,const Mat &img2);


TEST_CASE("Test ImageCropper 0."){
    Mat img = creatWhiteImage();
    double expected = 0.0;
    REQUIRE(blackRatio(img) <= expected);
}


TEST_CASE("Test ImageCropper 1."){
    Mat img = creatWhiteImage();
    double expected = 0.69;
    cropImage(img);
    REQUIRE(blackRatio(img) >= expected);
}

TEST_CASE("Test ImageCropper 2."){
    Mat img = creatWhiteImage();
    double expected = 1;
    cropImage(img);
    Mat croppedRect = img(aboveHorizon());
    REQUIRE(blackRatio(croppedRect) >= expected);
}
 

TEST_CASE("Test ImageCropper 3."){
    Mat temp = creatWhitePolyImage();
    Mat img = creatWhiteImage();
    cropImage(img);
    Mat res = combineImages(temp,img);
    double expected = 1.0; 
    SECTION("Test template") {
        REQUIRE(blackRatio(temp) < expected);
    }
   SECTION("After cropping") {
        REQUIRE(blackRatio(res) >= expected);
    }
}             

void cropImage(Mat &img){
    ImageCropper imageCropper = ImageCropper();
    imageCropper.setImage(img);
    imageCropper.cropRectangle(aboveHorizon());
    imageCropper.cropPolygon(initVehicleContour());
}

Mat combineImages(const Mat &img1,const Mat &img2){
    Mat result;
    bitwise_and(img1, img2, result);
    return result;
}


Mat creatWhiteImage(){
    Mat img = Mat::zeros(Size(640,480), CV_8UC1);      
    img = Scalar(255,255,255);
    return img;
}

Mat creatWhitePolyImage(){
    vector<Point> arr = initVehicleContour();
    Mat img = Mat::zeros(Size(640,480), CV_8UC1);
    img = Scalar(0,0,0);
    const cv::Point *pts = (const cv::Point*) cv::Mat(arr).data;
    int npts = Mat(arr).rows;
    fillPoly(img, &pts, &npts, 1, Scalar(255,255,255)); 
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
