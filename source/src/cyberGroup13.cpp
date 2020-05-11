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
#include <math.h>
#include <cmath>
#define PI 3.14159265

#include "ImageCropper.hpp"
#include "ImageFilter.hpp"
#include "ImageTracker.hpp"
#include "CsvManager.hpp"



using namespace cv;
using namespace std;

RNG rng(12345);
const String TEMPLATE_PATH = "templateCone1.png";

void initVehicleContour(std::vector<cv::Point> &vehicleContour, int width, int height);
Point calcPoint(Rect rect);
double getSteeringAngle(vector<Point> &leftCones,vector<Point> &rightCones);
bool intersection(Point a1, Point a2, Point b1, Point b2);
double cross(Point v1,Point v2);


int32_t main(int32_t argc, char **argv) {
    int32_t retCode{1};
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

            ImageTracker coneTracker = ImageTracker(TEMPLATE_PATH, 0);

            CsvManager::refresh();  //clean the directory & create a csv file

            opendlv::proxy::GroundSteeringRequest gsr;
            mutex gsrMutex;
            auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env){
                // The envelope data structure provide further details, such as sampleTimePoint as shown in this test case:
                // https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestEnvelopeConverter.cpp#L31-L40
                lock_guard<std::mutex> lck(gsrMutex);
                gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
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
                // TODO: Here, you can add some code to check the sampleTimePoint when the current frame was captured.
                uint32_t ts = cluon::time::toMicroseconds(sharedMemory->getTimeStamp().second);
                sharedMemory->unlock();
                
                imageCropper.setImage(img);
                imageCropper.cropRectangle(aboveHorizon);
                imageCropper.cropPolygon(vehicleContour);

                Mat yellowEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::yellowRanges));
                Mat blueEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::blueRanges));

                cv::Point blueCone, yellowCone, orangeCone;

                
                // Display images on your screen.
                if (VERBOSE) {

                    Scalar color = cv::Scalar(255,0,0);
                    vector<Rect> rectBlue, rectYellow;
                    coneTracker.setMinRectArea(75);
                    coneTracker.run(blueEdges, rectBlue);
                    coneTracker.run(yellowEdges, rectYellow);
                    
                    std::vector<cv::Point> yPoints,bPoints;
                    for (size_t k = 0; k<rectYellow.size(); k++){
                        yPoints.push_back(calcPoint(rectYellow[k]));
                    }
                    for (size_t j=0;j<rectBlue.size();j++){
                        bPoints.push_back(calcPoint(rectBlue[j]));
                    }
                    blueEdges = Scalar(0,0,0);
                    polylines(blueEdges,bPoints,false,Scalar(255,255,255),2,150,0);
                    yellowEdges = Scalar(0,0,0);
                    polylines(yellowEdges,yPoints,false,Scalar(255,255,255),2,150,0);
                    
                    vector<Vec4i> ylines;
                    HoughLinesP(yellowEdges, ylines, 1, CV_PI/180, 10, 10, 50);
                    for( size_t i = 0; i < ylines.size(); i++ )
                    {
                        line( img, Point(ylines[0][0], ylines[0][1]),
                            Point(ylines[0][2], ylines[0][3]), Scalar(0,0,255), 3, 8 );
                            break;
                    }
                    vector<Vec4i> blines;
                    HoughLinesP(blueEdges, blines, 1, CV_PI/180, 10, 10, 50);
                    for( size_t i = 0; i < blines.size(); i++ )
                    {
                        line( img, Point(blines[0][0], blines[0][1]),
                            Point(blines[0][2], blines[0][3]), Scalar(0,0,255), 3, 8 );
                            break;
                    }
                    


                    //if (blines.size() > 1 && ylines.size() > 1){
                    //    std::vector<cv::Point> rightPoints = {Point(blines[0][0],blines[0][1]),Point(blines[0][2],blines[0][3])};
                    //    std::vector<cv::Point> leftPoints = {Point(ylines[0][0],ylines[0][1]),Point(ylines[0][2],ylines[0][3])};
                    //    //cout << "left points >>>\n" << leftPoints << endl;
                    //    //cout << "right points >>>\n" << rightPoints << endl;
//
                    //    double gsr1 = getSteeringAngle(leftPoints,rightPoints);
                    //    //Here we log the data to the csv file
                    //    CsvManager::add(ts, gsr.groundSteering(), gsr1,"1");
//
                    //}
                    //else{
                    //    //CsvManager::add(ts, gsr.groundSteering(), 0.0,"0");
                    //}

                    //if (blines.size()>0 && ylines.size()>0){
                    //    std::vector<cv::Point> rightPoints = {Point(blines[0][0],blines[0][1]),Point(blines[0][2],blines[0][3])};
                    //    std::vector<cv::Point> leftPoints = {Point(ylines[0][0],ylines[0][1]),Point(ylines[0][2],ylines[0][3])};
                    //    double gr1 = getSteeringAngle(leftPoints,rightPoints);
                    //    CsvManager::add(ts, gsr.groundSteering(), gr1);
                    //}
                    
                    

                    line( img, Point(320, 290),
                            Point(320, 362), Scalar(0,255,255), 3, 8 );
                    
                    int x = (blines.size()>0 && ylines.size()>0)?1:(blines.size()>0)?2:(ylines.size()>0)?3:0;
                    int  b_x2, y_x2,x1,x2,y1,y2;
                    double x_offset , y_offset;
                    int mid = 320;
                    y_offset = 70;
                    switch (x)
                    {
                    case 1:
                        cout << "case 1" << endl;
                        cout << "yellow line at: " << ylines[0] << endl;
                        cout << "bluline at: " << blines[0] << endl;
                        b_x2 = blines[0][2];
                        y_x2 = ylines[0][2];
                        x_offset = (b_x2 + y_x2) / 2 - mid;
                        break;
                    case 2:
                        cout << "case 2" << endl;
                        cout << "bluline at: " << blines[0] << endl;
                        if(intersection(Point(320, 290),Point(320, 362),Point(blines[0][0],blines[0][2]),Point(blines[0][1],blines[0][3]))){
                            cout << "intersection with blue" << endl;
                            x1 = blines[0][0];
                            x2 = blines[0][2];
                            y1 = blines[0][1];
                            y2 = blines[0][3];
                            y_offset = abs(290-y1>0?290-y1:1);
                            x_offset = y_offset != 1 ? x2 - x1 : 0;
                            cout << "norm length: " << y_offset << endl;
                        }
                        else{
                                x_offset = 0.0;
                        }
                        
                        break;
                    case 3:
                        cout << "case 3" << endl;
                        cout << "yellow line at: " << ylines[0] << endl;
                        if(intersection(Point(320, 290),Point(320, 362),Point(ylines[0][0],ylines[0][2]),Point(ylines[0][1],ylines[0][3]))){
                            cout << "intersection with yellow" << endl;
                            x1 = ylines[0][0];
                            x2 = ylines[0][2];
                            y1 = ylines[0][1];
                            y2 = ylines[0][3];
                            
                            y_offset = abs(290-y1>0?290-y1:1);
                            x_offset = y_offset != 1 ? x2 - x1 : 0;
                            cout << "norm length: " << y_offset << endl;
                        }
                        else{
                            x_offset = 0.0;
                        }
                        break;
                    default:
                        break;
                    }
                    if(x){
                        //EPS: 1e-8
                    double angle_to_mid_radian = x_offset !=0 ? atan2(y_offset,x_offset) : 0;  //# angle (in radian) to center vertical line
                    double angle_to_mid_deg =  angle_to_mid_radian * 180.0 / PI;  //# angle (in degrees) to center vertical line
                    CsvManager::add(ts, gsr.groundSteering(), angle_to_mid_radian * 0.1, to_string(x));
                    }
                    else {
                        //CsvManager::add(ts, gsr.groundSteering(), 0.0, "0");
                    }
            

                    /*
                    for( size_t i = 0; i < rectBlue.size(); i++ ) {
                        rectangle( img, rectBlue[i].tl(), rectBlue[i].br(), color, 2 );   
                    }                     
                    
                    color = cv::Scalar(255,255,0);
                    for( size_t i = 0; i < rectYellow.size(); i++ ) {
                        rectangle( img, rectYellow[i].tl(), rectYellow[i].br(), color, 2 );   
                    } 
                    */

                    cv::imshow("/tmp/img/full", img);
                    cv::waitKey(1);
                }
            }
        }
        retCode = 0;
    }
    return retCode;
}

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

double getSteeringAngle(vector<Point> &leftCones,vector<Point> &rightCones)
{
    double leftSlope=(double)(leftCones[1].y-leftCones[0].y)/(double)(leftCones[1].x-leftCones[0].x);
    //The line format: Y=leftSlope*(X-X0)+X0
    double rightSlope=(double)(rightCones[1].y-rightCones[0].y)/(double)(rightCones[1].x-rightCones[0].x);
    //The line format: Y=rightSlope*(X-X0)+X0
    int middleOfTheCar=320;//I don't know the proper number yet
    int horizion=70;//I don't know the proper number yet
    double leftLaneIntersection=leftSlope*(middleOfTheCar-leftCones[0].x)+leftCones[0].x;
    double rightLaneIntersection=rightSlope*(middleOfTheCar-rightCones[0].x)+rightCones[0].x;
    if(leftLaneIntersection>horizion||rightLaneIntersection>horizion)
        if(leftLaneIntersection>rightLaneIntersection)
            return -leftLaneIntersection*0.0005;
        else
            return rightLaneIntersection*0.0005;
    return 0;
}

Point calcPoint(Rect rect){
    return (rect.br() + rect.tl()) * 0.5;
}


bool intersection(Point a1, Point a2, Point b1, Point b2)
{
    Point p = a1;
    Point q = b1;
    Point r(a2-a1);
    Point s(b2-b1);

    if(cross(r,s) == 0) {return false;}

    double t = cross(q-p,s)/cross(r,s);

    //intPnt = p + t*r;
    return true;
}

double cross(Point v1,Point v2){
    return v1.x*v2.y - v1.y*v2.x;
}