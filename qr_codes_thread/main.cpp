/*
 * Main file for starting a camera thread
 */
#include "camera.h"

#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

const int cameraDevice = 0;

void RunCamera()
{
    Camera camera(cameraDevice);

    while (true)
    {
	cv::Mat img = camera.TakePicture();
	//img = camera.ConvertToBinary(img);
	camera.decodeFrame(img);
	cv::waitKey(1);
    }
}

int main()
{
    std::thread cameraThread(RunCamera);
    cameraThread.join();
    
    return 0;
}
