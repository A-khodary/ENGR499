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
    ~Camera();

    // Returns true if capture was opened successfully
    bool OpenVideoCap(int cameraDevice);

    // Takes an image and returns it.
    // This function is thread safe
    cv::Mat TakePicture();

    // Shows the image with the given title
    void ShowImage(const std::string& title, const cv::Mat& image);
    
 private:
    cv::VideoCapture cap;
    std::mutex* takePictureLock;
    std::mutex* showImageLock;
};

#endif

