#include "ImageTracker.hpp"
using namespace cv;
using namespace std;

void ImageTracker::ImageTracker(const Mat &initialFrame) {
    Ptr<Tracker> tracker = TrackerKCF.create();
    tracker.init(initialFrame,)
}



