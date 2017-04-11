/*
 * Main file for starting threads
 */
#include "camera.h"
#include "qr_camera.h"
#include "shape_camera.h"
#include "imu.h"
#include "fansController.h"

#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

const int cameraDevice = 0;

void RunQRCamera(Camera* camera)
{
    QRCamera qrCamera(camera);

    while (true)
    {
	//std::cout << "Cheese" << std::endl;
	cv::Mat img = qrCamera.TakePicture();
	qrCamera.decodeFrame(img);
	cv::waitKey(100);
    }
}

void RunShapeCamera(Camera* camera)
{
    ShapeCamera shapeCamera(camera);

    while (true)
    {
	cv::Mat shapeImg = shapeCamera.TakePicture();
	shapeImg = shapeCamera.ThresholdImage(shapeImg);
	shapeCamera.RecognizeShapes(shapeImg);

	cv::waitKey(100);
    }
}

void RunIMU()
{
    IMUExecution();
}

void RunFans()
{
    FanExecution();
}

int main()
{
    // Global camera object referenced by camera threads
    Camera* camera = new Camera();
    if (!camera->OpenVideoCap(cameraDevice))
    {
	std::cout << "Video capture is not opened" << std::endl;
	return 1;
    }
    
    // Start the QR camera thread
    std::thread qrCameraThread(RunQRCamera, camera);

    // Start the Shape camera thread
    std::thread shapeCameraThread(RunShapeCamera, camera);

    // Start the IMU thread
    std::thread imuThread(RunIMU);

    // Start the fan thread
    std::thread fanThread(RunFans);
    
    qrCameraThread.join();
    shapeCameraThread.join();
    imuThread.join();
    fanThread.join();

    delete camera;
    
    return 0;
}
