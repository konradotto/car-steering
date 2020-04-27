#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H
#include <utility>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;
class ImageFilter {
public:
    ImageFilter();
    //void createFilters(const vector<Mat> filter, Mat &img);
    void applyFilters(Mat &img);

private:

    pair <Scalar,Scalar> blue1;
    pair <Scalar,Scalar> blue2;
    pair <Scalar,Scalar> yellow;

    Mat applyBlue1(const Mat img,pair<Scalar, Scalar> pair);
    Mat applyBlue2(const Mat img,pair<Scalar, Scalar> pair);
    Mat applyYellow(const Mat img,pair<Scalar, Scalar> pair);
};
#endif