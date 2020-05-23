#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include <opencv2/imgproc/imgproc.hpp>

/**
 * Class for marking areas on an image as well as cropping them from the image
 */
class ImageCropper {
    public:
        /**
         * Create instance of image cropper specifying the colors for marking and cropping
         * 
         * @param markColor_ color used by this specific ImageCropper for marking 
         * @param cropColor_ color used by this specific ImageCropper for cropping
         */
        ImageCropper(const cv::Scalar markColor_, const cv::Scalar cropColor_);

        /**
         * Create instancde of image cropper which will use the default colors for marking and cropping
         */
        ImageCropper() {};

        /**
         * Set the image that ImageCropper is supposed to be working on
         * 
         * @param img reference to the image that is being worked on (not a copy)
         */
        void setImage(cv::Mat &img);

        /**
         * Mark a rectangle on the image by changing the color of its border to markColor
         * 
         * @param markRect Rect specifying size and position of the area that should be marked 
         */
        void markRectangle(const cv::Rect &markRect);

        /**
         * Crop a rectangle out of the image by filling it with cropColor
         * 
         * @param cropRect Rect specifying size and position of the area that should be cropped
         */
        void cropRectangle(const cv::Rect &cropRect);

        /**
         * Mark a polygon by outlining its contour using markColor
         * 
         * @param markContour contour of the region that should be marked on image
         */
        void markPolygon(const std::vector<cv::Point> &markContour);
        /**
         * Crop a Polygon out of the image by filling it with cropColor
         * 
         * @param cropContour contour of the region that is to be cropped from image 
         */
        void cropPolygon(const std::vector<cv::Point> &cropContour);
    private:
        cv::Mat image {};
        cv::Scalar markColor = cv::Scalar(0,0,255,255); // Red color 
        cv::Scalar cropColor = cv::Scalar(0,0,0);       // Black color
};

const cv::Point * transformVectorToArray(const std::vector<cv::Point> &vector, int &nrOfArrayPoints);
#endif