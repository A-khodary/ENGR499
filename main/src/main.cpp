/*
 * Main file for starting a camera thread
 */
#include "qr_camera.h"

#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

const int cameraDevice = 0;

void RunCamera()
{
    QRCamera qrCamera(cameraDevice);

    while (true)
    {
	cv::Mat img = qrCamera.TakePicture();
	qrCamera.decodeFrame(img);
	cv::waitKey(1);
    }
}

int main()
{
    std::thread qrCameraThread(RunCamera);
    qrCameraThread.join();
    
    return 0;
}
