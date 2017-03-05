// Class for taking pictures
#ifndef CAMERA 
#define CAMERA

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class Camera
{
 public:
    Camera(int deviceNum, std::string QRImage);
    cv::Mat TakePicture();
    cv::Mat ConvertToBinary(cv::Mat img);
    
 private:
    int imgCounter;
    std::string filePrefix, fileSuffix;
    cv::VideoCapture cap;
    cv::Mat qrImage;
};

#endif
