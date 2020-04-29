#include "ImageFilter.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

const pair <Scalar,Scalar> ImageFilter::blue1                = {Scalar(100,150,0),Scalar(140,255,255)};
const pair <Scalar,Scalar> ImageFilter::blue2                = {Scalar(102, 117, 35),Scalar(145, 255, 255)};
const pair <Scalar,Scalar> ImageFilter::yellow               = {Scalar(16, 0, 69),Scalar(30, 255, 255)};
const vector<pair<Scalar, Scalar>> ImageFilter::blueRanges   = {ImageFilter::blue1, ImageFilter::blue2};
const vector<pair<Scalar, Scalar>> ImageFilter::yellowRanges = {ImageFilter::yellow};


Mat ImageFilter::filterColorRange(const Mat &img, vector<pair<Scalar, Scalar>> colorRanges) {
	// convert img to hsv-color-spectrum
	Mat hsv, filteredImage,filteredEdges;
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