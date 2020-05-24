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

#include "ImageCropper.hpp"
#include "ImageFilter.hpp"
#include "ImageTracker.hpp"
#include "CsvManager.hpp"
#include "ImageSaver.hpp"

using namespace cv;
using namespace std;

RNG rng(12345);

void initVehicleContour(std::vector<cv::Point> &vehicleContour, int width, int height);
Point calcPoint(Rect rect);
double getSteeringAngle(vector<Point> &leftCones,vector<Point> &rightCones);
bool intersection(Vec4i line, Point &x);
double cross(Point v1,Point v2);
double calcGSR(vector<Vec4i> bLines, vector<Vec4i> yLines);
void houghLines(vector<Vec4i> &bLines, vector<Vec4i> &yLines,const vector<Point> &bPoints,const vector<Point> &yPoints, Mat mat);
void calcOffset(const Vec4i &line,const Point intersection, int &xOffset, int &yOffset);
double stabilizedGSR(double nGsr);


const Point heading0 = {320,270};
const Point heading1 = {320,360};
Point ix = {0,0};
double currentAngle;
//const double intersectionDistance = 70;

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{1};
    uint32_t ts = 0;
    // Parse the command line parameters as we require the user to specify some mandatory information on startup.
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("cid")) ||
         (0 == commandlineArguments.count("name")) ||
         (0 == commandlineArguments.count("width")) ||
         (0 == commandlineArguments.count("height")) ) {
        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> --name=<name of shared memory area> [--verbose]" << std::endl;
        std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
        std::cerr << "         --name:   name of the shared memory area to attach" << std::endl;
        std::cerr << "         --width:  width of the frame" << std::endl;
        std::cerr << "         --height: height of the frame" << std::endl;
        std::cerr << "Example: " << argv[0] << " --cid=253 --name=img --width=640 --height=480 --verbose" << std::endl;
    }
    else {
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
            const cv::Rect aboveHorizon = cv::Rect(0, 0, WIDTH, (int) (0.52 * HEIGHT));
            std::vector<cv::Point> vehicleContour;
            initVehicleContour(vehicleContour, WIDTH, HEIGHT);

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
                ts = cluon::time::toMicroseconds(sharedMemory->getTimeStamp().second);
                // TODO: Here, you can add some code to check the sampleTimePoint when the current frame was captured.
                sharedMemory->unlock();
                
                
                // ImageSaver::run(img, aboveHorizon, vehicleContour);
                
                imageCropper.setImage(img);
                imageCropper.cropRectangle(aboveHorizon);//Crop the upper rectangle
                imageCropper.cropPolygon(vehicleContour);//Crop the vehicle

                Mat yellowEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::yellowRanges));
                Mat blueEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::blueRanges));

                cv::Point blueCone, yellowCone, orangeCone;
                vector<Rect> rectBlue, rectYellow;
                coneTracker.setMinRectArea(75);
                coneTracker.run(blueEdges, rectBlue);
                coneTracker.run(yellowEdges, rectYellow);
                

                vector<Point> yPoints,bPoints;
                for (size_t k = 0; k<rectYellow.size(); k++){
                    yPoints.push_back(calcPoint(rectYellow[k]));
                }
                for (size_t j=0;j<rectBlue.size();j++){
                    bPoints.push_back(calcPoint(rectBlue[j]));
                }
                vector<Vec4i> bLines,yLines;
                houghLines(bLines,yLines,bPoints,yPoints,blueEdges);
                double gsr1 = calcGSR(bLines,yLines);
                CsvManager::add(ts, gsr.groundSteering(), gsr1);
                currentAngle = gsr1;

                // Display images on your screen.
                if (VERBOSE) {

                    for(size_t i = 0; i < yLines.size(); i++){
                        line( img, Point(yLines[i][0], yLines[i][1]),
                            Point(yLines[i][2], yLines[i][3]), Scalar(0,0,255), 3, 8 );
                            break;
                    }
                    for(size_t i = 0; i < bLines.size(); i++){
                        line(img, Point(bLines[i][0], bLines[i][1]),
                            Point(bLines[i][2], bLines[i][3]), 
                            Scalar(0,0,255), 3, 8);
                            break;
                    }
                    
                    line(img, heading0, heading1, Scalar(0,255,255), 3, 8);
                    if (ix.x != 0 && ix.y != 0){
                        circle(img, ix, 3, {255,0,0}, CV_FILLED);
                    }

                    cv::imshow("/tmp/img/full", img);
                    cv::waitKey(1);
                }


                // print the desired output to console
                std::clog << "group_13;" << ts << ";" << gsr1 << std::endl;
            }
        }
        retCode = 0;
    }
    return retCode;
}

//Vehicle point outline
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
    //return (norm(Mat(x),Mat(heading1)) <= intersectionDistance);
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

double calcGSR(vector<Vec4i> bLines, vector<Vec4i> yLines){
    
    int x = (bLines.size()>0 && yLines.size()>0)?1:(bLines.size()>0)?2:(yLines.size()>0)?3:0;
    int xOffset = 0 , yOffset = 1;
    Point intersectionPoint,intersectionPoint2;
    switch (x)
    {
    case 1:
        if (intersection(bLines[0],intersectionPoint) && intersection(yLines[0],intersectionPoint2)){
            double distBlue = norm(Mat(heading1),Mat(intersectionPoint));
            double distYellow = norm(Mat(heading1),Mat(intersectionPoint2));
            calcOffset(distBlue<distYellow?bLines[0]:yLines[0],distBlue<distYellow?intersectionPoint:intersectionPoint2,xOffset,yOffset);
            break;
        }
        else if(intersection(bLines[0],intersectionPoint)){
            calcOffset(bLines[0],intersectionPoint,xOffset,yOffset);
        }
        else if (intersection(yLines[0],intersectionPoint)){
            calcOffset(yLines[0],intersectionPoint,xOffset,yOffset);
        }
        break;
    case 2:
        if(intersection(bLines[0],intersectionPoint)){
            calcOffset(bLines[0],intersectionPoint,xOffset,yOffset);
        }
        break;
    case 3:
        if(intersection(yLines[0],intersectionPoint)){
            calcOffset(yLines[0],intersectionPoint,xOffset,yOffset);
        }
        break;
    default:
        break;
    }
    double theta = atan2(yOffset,xOffset);  //# angle (in radian) to center vertical line
    return ((theta>1||theta<-1)?0:(theta<-0.3?-0.3:theta>0.3?0.3:theta));
    

}

void calcOffset(const Vec4i &line,const Point intersection, int &xOffset, int &yOffset){
    xOffset = line[2] - intersection.x;
    yOffset = line[3] - intersection.y;
}
