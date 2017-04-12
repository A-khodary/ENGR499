#include "camera.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <mutex>

Camera::Camera()
{
    takePictureLock = new std::mutex();
    showImageLock = new std::mutex();
}

Camera::~Camera()
{
    delete takePictureLock;
    delete showImageLock;
}

bool Camera::OpenVideoCap(int cameraDevice)
{
    cap = cv::VideoCapture(cameraDevice);
    if (!cap.isOpened())
    {
	std::cout << "Video capture is not opened" << std::endl;
	return false;
    }

    return true;
}

cv::Mat Camera::TakePicture()
{
    cv::Mat img;

    takePictureLock->lock();
    if (!cap.read(img))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }
    takePictureLock->unlock();

    return img;
}

void Camera::ShowImage(const std::string& title, const cv::Mat& image)
{
    showImageLock->lock();
    imshow(title, image);
    showImageLock->unlock();
}
