#include "camera.h"

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

Camera::Camera(int deviceNum, std::string QRImage)
{
    cap = cv::VideoCapture(deviceNum);
    //cap = cv::VideoCapture("gst-launch-1.0 nvcamerasrc ! 'video/x-raw(memory:NVMM), width=(int)1920, height=(int)1080, format=(string)I420, framerate=(fraction)30/1' ! nvtee ! nvoverlaysink -e");
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

    qrImage = cv::imread(QRImage);
}

cv::Mat Camera::TakePicture()
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

    return img;
}

cv::Mat Camera::ConvertToBinary(cv::Mat img)
{
    cv::cvtColor(img, img, CV_BGR2GRAY);

    cv::adaptiveThreshold(img, img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
            cv::THRESH_BINARY, 41, 15);

    imshow("binary", img);

    return img;
}
