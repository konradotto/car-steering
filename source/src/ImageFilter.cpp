#include "ImageFilter.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
//pair up hsv values for the various colors
const pair <Scalar,Scalar> ImageFilter::blue1                = {Scalar(100,150,0),Scalar(140,255,255)};
const pair <Scalar,Scalar> ImageFilter::blue2                = {Scalar(102, 117, 35),Scalar(145, 255, 255)};
const pair <Scalar,Scalar> ImageFilter::yellow               = {Scalar(16, 0, 69),Scalar(30, 255, 255)};
const pair <Scalar,Scalar> ImageFilter::orange               = {Scalar(10, 100, 20), Scalar(25, 255, 255)};

//Set vector pairs of type scalar to create the ranges needed. we combine blue 1 and blue 2 to 1 blue color
const vector<pair<Scalar, Scalar>> ImageFilter::blueRanges   = {ImageFilter::blue1, ImageFilter::blue2};
const vector<pair<Scalar, Scalar>> ImageFilter::yellowRanges = {ImageFilter::yellow};
const vector<pair<Scalar, Scalar>> ImageFilter::orangeRanges = {ImageFilter::orange};


Mat ImageFilter::filterColorRange(const Mat &img, vector<pair<Scalar, Scalar>> colorRanges) {
	// convert img to hsv-color-spectrum
	Mat hsv, filteredImage,filteredEdges,res;
	cvtColor(img, hsv, COLOR_BGR2HSV);//Converts colorspace from BGR to HSV

	// iterate over color ranges
	bool firstIteration = true;
	
	for (auto const& colorRange: colorRanges) {//For each color range in color ranges
		if (firstIteration) {
			filteredImage = applyColorFilter(hsv, colorRange);//apply filter to the image
			res = filteredImage;
			firstIteration = false;//break first if by setting to false
		} else {
			bitwise_or(filteredImage,applyColorFilter(hsv, colorRange),res);
			filteredImage = res;
		}
	}
	return res;
}


Mat ImageFilter::filterEdges(const Mat &img) {
	Mat filteredEdges;
	Canny(img, filteredEdges, 100, 200);
	return filteredEdges;
}

Mat ImageFilter::applyColorFilter(const Mat img, pair<Scalar, Scalar> pair){
	Mat filtered;
	inRange(img, pair.first, pair.second, filtered);
	
	return filtered;
}