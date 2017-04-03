#include "camera.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <mutex>

Camera::Camera()
{

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

cv::Mat Camera::TakePicture(bool showImg)
{
    cv::Mat img;

    lock.lock();
    if (!cap.read(img))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }
    lock.unlock();

    if (showImg)
    {
	imshow("Image", img);
    }

    return img;
}
