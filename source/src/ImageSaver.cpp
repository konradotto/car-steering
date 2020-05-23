#include "ImageSaver.hpp"

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
const string ImageSaver::matches = "Matches";
const string ImageSaver::base = "Unmerged";
const string ImageSaver::merged = "Merged";
const string ImageSaver::processed = "processed";
const string ImageSaver::type = ".png";

void ImageSaver::run(Mat const& image, Rect const& aboveHorizon, const vector<Point> &vehicleContour) {
    Mat img = image.clone();       // copy constant input image for further steps

    imwrite(dir+org+type, img);

    ImageCropper imageCropper = ImageCropper();
    ImageTracker coneTracker = ImageTracker(0);
     
    // mark cropping areas and save           
    imageCropper.setImage(img);
    imageCropper.markRectangle(aboveHorizon);
    imageCropper.markPolygon(vehicleContour);
    cv::imwrite(dir+mark+type, img);

    // crop out areas and save
    imageCropper.cropRectangle(aboveHorizon);
    imageCropper.cropPolygon(vehicleContour);
    cv::imwrite(dir+crop+type, img);

    Mat cropped = img.clone();

    // filter color ranges and save
    Mat yellowIm = ImageFilter::filterColorRange(img, ImageFilter::yellowRanges);
    cv::imwrite(dir+yellow+filtered+type, yellowIm);
    Mat blueIm = ImageFilter::filterColorRange(img, ImageFilter::blueRanges);
    cv::imwrite(dir+blue+filtered+type, blueIm);

    // reduce to edges and save
    Mat yellowEdges = ImageFilter::filterEdges(yellowIm);
    cv::imwrite(dir+yellow+edges+type, yellowEdges);
    Mat blueEdges = ImageFilter::filterEdges(blueIm);
    cv::imwrite(dir+blue+edges+type, blueEdges);

    // detect yellow cones and save
    Mat yellowDetections, blueDetections;
    vector<Rect> yellowMatches, blueMatches;

    Mat tempImg = yellowEdges.clone();     // copy of image to draw rectangles on
    yellowMatches = coneTracker.detectMatches(yellowEdges, yellowDetections);
    drawRects(yellowMatches, tempImg, Scalar(255,255,255,255));
    cv::imwrite(dir+yellow+matches+base+type, tempImg);

    // detect blue cones and save
    tempImg = blueEdges.clone();
    blueMatches = coneTracker.detectMatches(blueEdges, blueDetections);
    drawRects(blueMatches, tempImg, Scalar(255,0,0,255));
    cv::imwrite(dir+blue+matches+base+type, tempImg);

    // merge yellow overlapping and save
    tempImg = yellowEdges.clone();
    coneTracker.mergeOverlappingRectangles(yellowMatches);
    drawRects(yellowMatches, tempImg, Scalar(255,255,255,255));
    cv::imwrite(dir+yellow+matches+merged+type, tempImg);

    // merge blue overlapping and save
    tempImg = blueEdges.clone();
    coneTracker.mergeOverlappingRectangles(blueMatches);
    drawRects(blueMatches, tempImg, Scalar(255,0,0,255));
    cv::imwrite(dir+blue+matches+merged+type, tempImg);

    // filter yellow matches and save
    tempImg = cropped.clone();
    coneTracker.setMinRectArea(75);
    coneTracker.filterRectsByArea(yellowMatches, 75, 5000);
    coneTracker.filterRectsByDimensions(yellowMatches);
    drawRects(yellowMatches, tempImg, Scalar(0,255,255,255));
    cv::imwrite(dir+yellow+matches+filtered+type, tempImg);

    // filter blue matches and save
    tempImg = cropped.clone();
    coneTracker.filterRectsByArea(blueMatches, 75, 5000);
    coneTracker.filterRectsByDimensions(blueMatches);
    drawRects(blueMatches, tempImg, Scalar(255,0,0,255));
    cv::imwrite(dir+blue+matches+filtered+type, tempImg);

    // reunite and save
    tempImg = img.clone();
    drawRects(blueMatches, tempImg, Scalar(255,0,0,255));
    drawRects(yellowMatches, tempImg, Scalar(0,255,255,255));
    cv::imwrite(dir+processed+type, tempImg);
}

void ImageSaver::drawRects(const vector<Rect> &rects, Mat &img, const Scalar &color) {
    for (Rect rect: rects) {               
        rectangle(img, rect.tl(), rect.br(), color, 2 );
    }
}