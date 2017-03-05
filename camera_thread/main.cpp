/*
 * Main file for starting a camera thread
 */
#include "camera.h"

#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

const int cameraDevice = 0;
const std::string QRImageFile = "qr_code_one_square2.jpg";

void RunCamera()
{
    Camera camera(cameraDevice, QRImageFile);

    while (true)
    {
	cv::Mat img = camera.TakePicture();
	camera.ConvertToBinary(img);
	cv::waitKey(5000);
    }
}

int main()
{
    std::thread t(RunCamera);

    t.join();
    
    return 0;
}
