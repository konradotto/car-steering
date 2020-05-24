#include "ImageFilter.hpp"
#include "opencv2/opencv.hpp"
#include <utility>

using namespace cv;
using namespace std;

// pair up hsv values for the various colors
const pair <Scalar,Scalar> ImageFilter::blue1                = {Scalar(100,150,0),Scalar(140,255,255)};
const pair <Scalar,Scalar> ImageFilter::blue2                = {Scalar(102, 117, 35),Scalar(145, 255, 255)};
const pair <Scalar,Scalar> ImageFilter::yellow               = {Scalar(16, 0, 69),Scalar(30, 255, 255)};
const pair <Scalar,Scalar> ImageFilter::orange               = {Scalar(10, 100, 20), Scalar(25, 255, 255)};

// Set vector pairs of type scalar to create the ranges needed. we combine blue 1 and blue 2 to 1 blue color
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
		// if it is the first iteration, change the result to be the hsv image filtered for the current range
		if (firstIteration) {
			filteredImage = applyColorFilter(hsv, colorRange);//apply filter to the image
			res = filteredImage;
			firstIteration = false;//break first if-statement by setting to false

		// in all subsequent iterations, combine the result of filtering for the current range with the previous result 
		} else {
			bitwise_or(filteredImage, applyColorFilter(hsv, colorRange), res);
			filteredImage = res;
		}
	}
	return res;
}

// Apply edge filtering
Mat ImageFilter::filterEdges(const Mat &img) {
	Mat filteredEdges;
	Canny(img, filteredEdges, 100, 200); // Filter edges of an object using Canny
	return filteredEdges;
}

Mat ImageFilter::applyColorFilter(const Mat img, pair<Scalar, Scalar> pair){
	Mat filtered;
	inRange(img, pair.first, pair.second, filtered);	// apply lower and upper hsv values to the image and name it filtered

	return filtered;//return the filtered image
}