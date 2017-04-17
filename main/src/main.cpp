/*
 * Main file for starting threads
 */
#include "camera.h"
#include "qr_camera.h"
#include "shape_camera.h"
#include "imu.h"
#include "fansController.h"
#include <mutex>
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

void RunIMU(struct imuInfo &data, std::mutex &imuDataMutex)
{
    IMUExecution(data, imuDataMutex);
}

void RunFans()
{
    FanExecution();
}

void ReadData(struct imuInfo &data, std::mutex &imuDataMutex)
{
    while(1){	
    	imuDataMutex.lock();
    	printf("Pitch: %lf\r", data.pitch);
	fflush(stdout);
    	imuDataMutex.unlock();
   }
}


int main()
{
    imuInfo imuData;
    imuData.x=0;
    imuData.y=0; 
    imuData.z=0; 
    imuData.roll=0; 
    imuData.pitch=0; 
    imuData.yaw=0;
    std::mutex imuDataMutex;
    // Global camera object referenced by camera threads
    /*Camera* camera = new Camera();
    if (!camera->OpenVideoCap(cameraDevice))
    {
	std::cout << "Video capture is not opened" << std::endl;
	return 1;
    }
    */
    // Start the QR camera thread
    //std::thread qrCameraThread(RunQRCamera, camera);

    // Start the Shape camera thread
    //std::thread shapeCameraThread(RunShapeCamera, camera);

    // Start the IMU thread
    std::thread imuThread(&RunIMU, std::ref(imuData), std::ref(imuDataMutex));
    std::thread readDataThread(&ReadData, std::ref(imuData), std::ref(imuDataMutex));
    // Start the fan thread
    //std::thread fanThread(RunFans);
    
    //qrCameraThread.join();
    //shapeCameraThread.join();
    imuThread.join();
    readDataThread.join();
    //fanThread.join();

    //delete camera;
    
    return 0;
}
