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

#include "ImageCropper.hpp"
#include "ImageFilter.hpp"
#include "ImageTracker.hpp"
#include "CsvManager.hpp"
#include "ImageSaver.hpp"

using namespace cv;
using namespace std;

RNG rng(12345);
const String TEMPLATE_PATH = "templateCone1.png";

void initVehicleContour(std::vector<cv::Point> &vehicleContour, int width, int height);


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
                
                // ImageSaver::run(img, aboveHorizon, vehicleContour);
                
                imageCropper.setImage(img);
                imageCropper.cropRectangle(aboveHorizon);
                imageCropper.cropPolygon(vehicleContour);

                Mat yellowEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::yellowRanges));
                Mat blueEdges = ImageFilter::filterEdges(ImageFilter::filterColorRange(img, ImageFilter::blueRanges));

                cv::Point blueCone, yellowCone, orangeCone;

                // Here we log the data to the csv file
                CsvManager::add(ts, gsr.groundSteering(), 0.5);

                // Display images on your screen.
                if (VERBOSE) {

                    Scalar color = cv::Scalar(255,0,0);
                    vector<Rect> rectBlue, rectYellow;

                    coneTracker.setMinRectArea(75);
                    coneTracker.run(blueEdges, rectBlue);
                    coneTracker.run(yellowEdges, rectYellow);
                    
                    for( size_t i = 0; i < rectBlue.size(); i++ ) {
                        rectangle( img, rectBlue[i].tl(), rectBlue[i].br(), color, 2 );   
                    }                     
                    
                    color = cv::Scalar(0,255,255);
                    for( size_t i = 0; i < rectYellow.size(); i++ ) {
                        rectangle( img, rectYellow[i].tl(), rectYellow[i].br(), color, 2 );   
                    } 
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