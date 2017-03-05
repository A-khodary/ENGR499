// Class for taking pictures
#ifndef CAMERA 
#define CAMERA

#include <string>
#include <opencv2/highgui/highgui.hpp>

class Camera
{
 public:
    Camera(int deviceNum);
    void TakePicture();

 private:
    int imgCounter;
    std::string filePrefix, fileSuffix;
    cv::VideoCapture cap;
};

#endif
