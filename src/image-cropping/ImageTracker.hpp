#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

#include <utility>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class ImageTracker {
    public:
        ImageTracker(const Mat &initialFrame);
        void setFrame(Mat &img);

    private:
       // Tracker tracker;
};
#endif