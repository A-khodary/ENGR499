#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
  //cv::Mat img = cv::imread("example.jpg");
  cv::Mat img;
  cv::VideoCapture cap("gst-launch-1.0 nvcamerasrc ! 'video/x-raw(memory:NVMM), width=(int)1920, height=(int)1080, format=(string)I420, framerate=(fraction)30/1' ! nvtee ! nvoverlaysink -e");

  if (!cap.isOpened())
	{
	  std::cout << "video capture is not opened" << std::endl;
	  return 0;
	}

  //cv::VideoCapture cap(0);
  
  for (;;)
	{
	  if (!cap.read(img))
		{
		  std::cout << "Read failed" << std::endl;
		  break;
		}

	  cv::imshow("img", img);
	  char c = cv::waitKey(30);

	  if (c=='c')
		{
		  cap.release();
		  break;
		}
	}
}
