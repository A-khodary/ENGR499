// Class which contains the video capture shared by camera threads
#ifndef CAMERA
#define CAMERA

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <mutex>

class Camera
{
 public:
    Camera();

    // Returns true if capture was opened successfully
    bool OpenVideoCap(int cameraDevice);

    // Takes an image and returns it.
    // This function is thread safe
    // showImg: if true shows the image
    cv::Mat TakePicture(bool showImg);
    
 private:
    cv::VideoCapture cap;
    std::mutex lock;
};

#endif

