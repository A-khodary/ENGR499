/*
 * Main file for starting a camera thread
 */
#include "camera.h"

#include <thread>

void RunCamera()
{
    Camera camera(0);

    while (true)
    {
	camera.TakePicture();
	cv::waitKey(1000);
    }
}

int main()
{
    std::thread t(RunCamera);

    t.join();
    
    return 0;
}
