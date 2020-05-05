#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <utility>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class ImageFilter {

public:
    static Mat filterColorRange(const Mat &img, vector<pair<Scalar, Scalar>> colorRanges);
    static Mat filterEdges(const Mat &img);
    static const pair <Scalar,Scalar> blue1; 	
    static const pair <Scalar,Scalar> blue2;
    static const pair <Scalar,Scalar> yellow;
    static const vector<pair<Scalar, Scalar>> blueRanges;
    static const vector<pair<Scalar, Scalar>> yellowRanges;
private:
    static Mat applyColorFilter(const Mat img,pair<Scalar, Scalar> pair);
};
#endif
