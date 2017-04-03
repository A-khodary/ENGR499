// Class for analyzing images for qr codes
#ifndef QR_CAMERA
#define QR_CAMERA

#include "camera.h"

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <zbar.h>

class QRCamera
{
 public:
    QRCamera(Camera* camera);
    cv::Mat TakePicture();
    cv::Mat ConvertToBinary(cv::Mat img);
    void decodeFrame(cv::Mat frame);
    void configureScanner();
    
 private:
    int imgCounter;
    std::string filePrefix, fileSuffix;
    Camera* camera;
    zbar::ImageScanner scanner;
};

#endif
