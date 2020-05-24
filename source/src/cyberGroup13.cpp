/*
 * Copyright (C) 2020  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Include the single-file, header-only middleware libcluon to create high-performance microservices
#include "cluon-complete.hpp"
// Include the OpenDLV Standard Message Set that contains messages that are usually exchanged for automotive or robotic applications 
#include "opendlv-standard-message-set.hpp"

// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <cmath>

// Include our classes for image processing
#include "ImageCropper.hpp"
#include "ImageFilter.hpp"
#include "ImageTracker.hpp"
#include "CsvManager.hpp"
#include "ImageSaver.hpp"

// use namespaces std and cv (from OpenCV)
using namespace cv;
using namespace std;

static const int32_t NO_ERROR               = 0;
static const int32_t MISSING_CMD_ARGUMENTS  = 1;

static const int32_t NO_LINES               = 0;
static const int32_t BLUE_AND_YELLOW_LINE   = 1;
static const int32_t BLUE_LINE_ONLY         = 2;
static const int32_t YELLOW_LINE_ONLY       = 3;

static const double ANGLE_UPPER_LIMIT       = 0.152;

bool missingCommandLineArguments(auto arguments, char *argv[]);
void initVehicleContour(std::vector<cv::Point> &vehicleContour, int width, int height); // function to create a contour for cropping out the vehicle
Point calcPoint(Rect rect);
bool intersection(Vec4i line, Point &x);
double cross(Point v1,Point v2);
int checkIntersections(vector<Vec4i> bLines, vector<Vec4i> yLines, Point &interPoint1, Point &interPoint2);
double calcGSR(vector<Vec4i> bLines, vector<Vec4i> yLines);
void houghLines(vector<Vec4i> &bLines, vector<Vec4i> &yLines,const vector<Point> &bPoints,const vector<Point> &yPoints, Mat mat);
void calcOffset(const Vec4i &line,const Point intersection, int &xOffset, int &yOffset);


const Point heading0 = {320,270};
const Point heading1 = {320,360};
Point ix = {0,0};

int32_t main(int32_t argc, char **argv) {
    uint32_t ts = 0;

    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);

    // if parsing the command line arguments returns an error, end main with error code
    if (missingCommandLineArguments(commandlineArguments, argv)) {
        return MISSING_CMD_ARGUMENTS;
    }
    
    // Extract the values from the command line parameters
    const std::string NAME{commandlineArguments["name"]};
    const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
    const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
    const bool VERBOSE{commandlineArguments.count("verbose") != 0};

    // Attach to the shared memory.
    std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};
    if (sharedMemory && sharedMemory->valid()) {
        std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

        // Interface to a running OpenDaVINCI session where network messages are exchanged.
        // The instance od4 allows you to send and receive messages.
        cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

        ImageCropper imageCropper = ImageCropper();
        const cv::Rect aboveHorizon = cv::Rect(0, 0, WIDTH, (int) (0.52 * HEIGHT));//size of upper rectangle
        std::vector<cv::Point> vehicleContour;
        initVehicleContour(vehicleContour, WIDTH, HEIGHT);//Call function for outlining out the vehicle contour

        ImageTracker coneTracker = ImageTracker(0);

        CsvManager::refresh();  //clean the directory & create a csv file

        opendlv::proxy::GroundSteeringRequest gsr;
        mutex gsrMutex;
        auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env){
            // The envelope data structure provide further details, such as sampleTimePoint as shown in this test case:
            // https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestEnvelopeConverter.cpp#L31-L40
            lock_guard<std::mutex> lck(gsrMutex);
            gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
            //CsvManager::add(ts, gsr.groundSteering(), 0.0,"0");
        };
        od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);


        // Endless loop; end the program by pressing Ctrl-C.
        while (od4.isRunning()) {
            // OpenCV data structure to hold an image.
            cv::Mat img;

            // Wait for a notification of a new frame.
            sharedMemory->wait();

            // Lock the shared memory.
            sharedMemory->lock();
            
            {
                // Copy the pixels from the shared memory into our own data structure.
                cv::Mat wrapped(HEIGHT, WIDTH, CV_8UC4, sharedMemory->data());
                img = wrapped.clone();
            } 
            ts = cluon::time::toMicroseconds(sharedMemory->getTimeStamp().second);  // get the timestamp from shared memory
            sharedMemory->unlock();
            
            // crop image
            imageCropper.setImage(img);
            imageCropper.cropRectangle(aboveHorizon);   // Crop the upper rectangle
            imageCropper.cropPolygon(vehicleContour);   // Crop the vehicle

            // apply hsv-filter and canny edge detection
            Mat yellowEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::yellowRanges));
            Mat blueEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::blueRanges));

            // detect cones in blue and yellow image
            cv::Point blueCone, yellowCone, orangeCone;
            vector<Rect> rectBlue, rectYellow;
            coneTracker.setMinRectArea(75);
            coneTracker.run(blueEdges, rectBlue);
            coneTracker.run(yellowEdges, rectYellow);
            

            vector<Point> yPoints,bPoints; //Array storing yellow points and blue points
            for (size_t k = 0; k<rectYellow.size(); k++){
                yPoints.push_back(calcPoint(rectYellow[k]));
            }
            for (size_t j=0;j<rectBlue.size();j++){
                bPoints.push_back(calcPoint(rectBlue[j]));
            }

            // calculate houghLines
            vector<Vec4i> bLines,yLines;
            houghLines(bLines,yLines,bPoints,yPoints,blueEdges);

            // calculate a ground steering request based on houghLines
            double gsr1 = calcGSR(bLines,yLines);

            // Display images on your screen.
            if (VERBOSE) {

                // plot yellow lines
                for(size_t i = 0; i < yLines.size(); i++){
                    line( img, Point(yLines[i][0], yLines[i][1]),
                        Point(yLines[i][2], yLines[i][3]), Scalar(0,0,255), 3, 8 );
                        break;
                }

                // plot blue lines
                for(size_t i = 0; i < bLines.size(); i++){
                    line(img, Point(bLines[i][0], bLines[i][1]),
                        Point(bLines[i][2], bLines[i][3]), 
                        Scalar(0,0,255), 3, 8);
                        break;
                }
                
                // plot current direction
                line(img, heading0, heading1, Scalar(0,255,255), 3, 8);
                if (ix.x != 0 && ix.y != 0){
                    circle(img, ix, 3, {255,0,0}, CV_FILLED);       // plot a circle for the intersection
                }

                cv::imshow("/tmp/img/full", img);       // show the image
                cv::waitKey(1);
            }

            // store values for current frame in csv file
            CsvManager::add(ts, gsr.groundSteering(), gsr1);

            // print the desired output to console
            std::clog << "group_13;" << ts << ";" << gsr1 << std::endl;
        }
    }
    
    return NO_ERROR;
}

bool missingCommandLineArguments(auto arguments, char *argv[]) {
    bool missingArguments = false;

    // Print error message if any required argument is missing
    if ( (0 == arguments.count("cid")) ||
         (0 == arguments.count("name")) ||
         (0 == arguments.count("width")) ||
         (0 == arguments.count("height")) ) {
        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> --name=<name of shared memory area> [--verbose]" << std::endl;
        std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
        std::cerr << "         --name:   name of the shared memory area to attach" << std::endl;
        std::cerr << "         --width:  width of the frame" << std::endl;
        std::cerr << "         --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=253 --name=img --width=640 --height=480 --verbose" << std::endl;
        missingArguments = true;
    }

    return missingArguments;
}

// Vehicle point outline
void initVehicleContour(std::vector<cv::Point> &vehicleContour, int width, int height) {
    vehicleContour.push_back(cv::Point(0, height));
    vehicleContour.push_back(cv::Point(0, 423));
    vehicleContour.push_back(cv::Point(72, 406));
    vehicleContour.push_back(cv::Point(72, 446));
    vehicleContour.push_back(cv::Point(137, 426));
    vehicleContour.push_back(cv::Point(139, 391));
    vehicleContour.push_back(cv::Point(219, 374));
    vehicleContour.push_back(cv::Point(266, 373));
    vehicleContour.push_back(cv::Point(272, 362));
    vehicleContour.push_back(cv::Point(377, 363));
    vehicleContour.push_back(cv::Point(379, 369));
    vehicleContour.push_back(cv::Point(430, 372));
    vehicleContour.push_back(cv::Point(509, 397));
    vehicleContour.push_back(cv::Point(511, 433));
    vehicleContour.push_back(cv::Point(581, 453));
    vehicleContour.push_back(cv::Point(581, 412));
    vehicleContour.push_back(cv::Point(width, 427));
    vehicleContour.push_back(cv::Point(width, height));
}

Point calcPoint(Rect rect){
    Point n = (rect.br() + rect.tl()) * 0.5;
    return {n.x, rect.br().y};
}


bool intersection(Vec4i line, Point &x)
{
    Point lineP0 = {line[0],line[1]};
    Point lineP1 = {line[2],line[3]};
    Point r(heading1-heading0);
    Point s(lineP1-lineP0);

    if(abs(cross(r,s) - 0.0) <= 0.0000001) {return false;}

    double t = cross(lineP0-heading0,s)/cross(r,s);

    x = heading0 + t*r;
    ix = x;
    return (x.y >= heading0.y && x.y <= heading1.y);
}

double cross(Point v1,Point v2){
    return v1.x*v2.y - v1.y*v2.x;
}

void houghLines(vector<Vec4i> &bLines, vector<Vec4i> &yLines,const vector<Point> &bPoints,const vector<Point> &yPoints, Mat mat){
    mat = Scalar(0,0,0);
    polylines(mat,bPoints,false,Scalar(255,255,255),2,150,0);
    HoughLinesP(mat, bLines, 1, CV_PI/180, 10, 10, 10);
    mat = Scalar(0,0,0);
    polylines(mat,yPoints,false,Scalar(255,255,255),2,150,0);
    HoughLinesP(mat, yLines, 1, CV_PI/180, 10, 10, 10);
}

int checkIntersections(vector<Vec4i> bLines, vector<Vec4i> yLines, Point &interPoint1, Point &interPoint2) {
    int intersections = NO_LINES;
    int bSize = bLines.size();
    int ySize = yLines.size();

    if (bSize > 0 && ySize > 0 && intersection(bLines[0], interPoint1) && intersection(yLines[0], interPoint2)) {
        intersections = BLUE_AND_YELLOW_LINE;
    } else if (bSize > 0 && intersection(bLines[0], interPoint1)) {
        intersections = BLUE_LINE_ONLY;
    } else if (ySize > 0 && intersection(yLines[0], interPoint1)) {
        intersections = YELLOW_LINE_ONLY;    
    } 
    
    return intersections;
}

// Calculate the ground steering angle/request
double calcGSR(vector<Vec4i> bLines, vector<Vec4i> yLines){
    
    //int availableLines = (bLines.size() > 0 && yLines.size()>0) ? 1:(bLines.size()>0)?2:(yLines.size()>0)?3:0;
    int xOffset = 0 , yOffset = 1;
    Point intersectionPoint1, intersectionPoint2;
    double distBlue, distYellow;


    switch (checkIntersections(bLines, yLines, intersectionPoint1, intersectionPoint2)) {
        case BLUE_AND_YELLOW_LINE:
            distBlue = norm(Mat(heading1), Mat(intersectionPoint1));
            distYellow = norm(Mat(heading1), Mat(intersectionPoint2));
            calcOffset(distBlue<distYellow ? bLines[0] : yLines[0], distBlue<distYellow ? intersectionPoint1 : intersectionPoint2, xOffset, yOffset);
            break;
        case BLUE_LINE_ONLY:
            calcOffset(bLines[0], intersectionPoint1, xOffset, yOffset);
            break;
        case YELLOW_LINE_ONLY:
            calcOffset(yLines[0], intersectionPoint1, xOffset, yOffset);
            break;
        default:
            break;
    }
    double theta = atan2(yOffset, xOffset);  // angle (in radian) to center vertical line

    // filter out extreme values calculated for theta
    double absTheta = abs(theta);
    if (absTheta > 1) {
        theta = 0;
    } else if (absTheta > ANGLE_UPPER_LIMIT) {
        theta = copysign(ANGLE_UPPER_LIMIT, theta);     // limit to max 
    }

    return theta;
}

// calculate the vertical and horizontal offset between a line and a point
void calcOffset(const Vec4i &line, const Point intersection, int &xOffset, int &yOffset){
    xOffset = line[2] - intersection.x;
    yOffset = line[3] - intersection.y;
}