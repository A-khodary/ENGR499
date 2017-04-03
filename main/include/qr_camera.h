// Class for taking pictures
#ifndef QR_CAMERA
#define QR_CAMERA

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <zbar.h>

class QRCamera
{
 public:
    QRCamera(int deviceNum);
    cv::Mat TakePicture();
    cv::Mat ConvertToBinary(cv::Mat img);
    void decodeFrame(cv::Mat frame);
    void configureScanner();
    
 private:
    int imgCounter;
    std::string filePrefix, fileSuffix;
    cv::VideoCapture cap;
    zbar::ImageScanner scanner;
};

#endif
