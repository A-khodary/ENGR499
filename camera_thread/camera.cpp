#include "camera.h"

#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>

Camera::Camera(int deviceNum)
{
    cap = cv::VideoCapture(deviceNum);
    if (!cap.isOpened())
    {
	std::cout << "Video capture is not opened" << std::endl;
	throw cv::Exception();
    }

    if (!cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920))
	std::cout << "Resizing failed" << std::endl;
    if (!cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080))
	std::cout << "Resizing failed" << std::endl;
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    
    imgCounter = 0;
    filePrefix = "images/img";
    fileSuffix = ".jpg";
}

void
Camera::TakePicture()
{
    cv::Mat img;
    if (!cap.read(img))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }
    
    std::string filename = filePrefix + std::to_string(imgCounter++)
	+ fileSuffix;
    imshow("image", img);
    imwrite(filename, img);
}
