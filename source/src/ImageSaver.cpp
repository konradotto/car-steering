#include "ImageSaver.hpp"

const String TEMPLATE_PATH = "templateCone1.png";

using namespace std;
using namespace cv;

const string ImageSaver::dir = "/img/";
const string ImageSaver::org = "unprocessed";
const string ImageSaver::mark = "markCropAreas";
const string ImageSaver::crop = "cropped";
const string ImageSaver::blue = "blue";
const string ImageSaver::yellow = "yellow";
const string ImageSaver::filtered = "Filtered";
const string ImageSaver::edges = "Edges";
const string ImageSaver::type = ".png";

void ImageSaver::run(const Mat &image, const Rect &aboveHorizon, const vector<Point> &vehicleContour) {
    Mat img = Mat(image);       // copy constant input image for further steps

    imwrite(dir+org+type, img);

    ImageCropper imageCropper = ImageCropper();
    ImageTracker coneTracker = ImageTracker(TEMPLATE_PATH, 0);
     
    // mark cropping areas           
    imageCropper.setImage(img);
    imageCropper.markRectangle(aboveHorizon);
    imageCropper.markPolygon(vehicleContour);
    cv::imwrite(dir+mark+type, img);

    // crop out areas
    imageCropper.cropRectangle(aboveHorizon);
    imageCropper.cropPolygon(vehicleContour);
    cv::imwrite(dir+crop+type, img);

    // filter color ranges
    Mat yellowIm = ImageFilter::filterColorRange(img, ImageFilter::yellowRanges);
    cv::imwrite(dir+yellow+filtered+type, yellowIm);
    Mat blueIm = ImageFilter::filterColorRange(img, ImageFilter::blueRanges);
    cv::imwrite(dir+blue+filtered+type, blueIm);

    // reduce to edges 
    Mat yellowEdges = ImageFilter::filterEdges(yellowIm);
    cv::imwrite(dir+yellow+edges+type, yellowEdges);
    Mat blueEdges = ImageFilter::filterEdges(blueIm);
    cv::imwrite(dir+blue+edges+type, blueEdges);

    
}