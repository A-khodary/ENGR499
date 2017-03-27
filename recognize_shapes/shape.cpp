#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Mat GetImage(cv::VideoCapture& cap)
{
    // Take image
    cv::Mat shapeImg;
    if (!cap.read(shapeImg))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }

    //imwrite("image.jpg", shapeImg);
    imshow("shapeImg", shapeImg);

    return shapeImg;
}

cv::Mat ThresholdImage(cv::Mat shapeImg)
{
    // Convert to binary
    cv::cvtColor(shapeImg, shapeImg, CV_BGR2GRAY);

    imshow("Grayscale", shapeImg);
    
    cv::adaptiveThreshold(shapeImg, shapeImg, 255,
    			  cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 51, 15);
    
    //cv::threshold(shapeImg, shapeImg, 50, 255, CV_THRESH_BINARY);

    imshow("Threshold", shapeImg);

    return shapeImg;
}

void RecognizeShapes(cv::Mat shapeImg)
{
    int thresh = 100;
    int max_thresh = 200;

    Canny(shapeImg, shapeImg, thresh, max_thresh, 3);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::RNG rng(12345);
    findContours(shapeImg, contours, hierarchy, CV_RETR_TREE,
		 CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    
    // Draw contours
    cv::Mat drawing = cv::Mat::zeros(shapeImg.size(), CV_8UC3);

    /*
    for (int i = 0; i < contours.size(); i++)
    {
	cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());     
    }

    imshow("Result window", drawing);
    */

    std::vector<cv::Point> approxShape;
    for (int i = 0; i < contours.size(); i++)
    {
	approxPolyDP(contours[i], approxShape,
		     arcLength(cv::Mat(contours[i]), true) * 0.05, true);

	if (approxShape.size() == 4 && cv::contourArea(approxShape) > 50)
	{
	    double perimeter = cv::arcLength(approxShape, true);
	    double sideLength = perimeter / 4;
	    double approxArea = pow(sideLength, 2);

	    double ratio = approxArea / cv::contourArea(approxShape);
	    if (ratio > 0.95 && ratio <  1.05)
	    {
		drawContours(drawing, contours, i, cv::Scalar(0, 255, 255), CV_FILLED);
		std::vector<cv::Point>::iterator vertex;
		for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		{
		    cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		}
	    }
	}
	
	/*if (approxShape.size() > 2 && cv::contourArea(approxShape) > 2500)
	{
	    if (approxShape.size() == 4)
	    {
		//std::cout << "Area: " << cv::contourArea(approxShape) << std::endl;
		
		drawContours(drawing, contours, i, cv::Scalar(0, 255, 255), CV_FILLED);
		std::vector<cv::Point>::iterator vertex;
		for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		{
		    cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		}
	    }
	    else if (approxShape.size() == 3)
	    {
		//std::cout << "Area: " << cv::contourArea(approxShape) << std::endl;
		
		drawContours(drawing, contours, i, cv::Scalar(255, 0, 255), CV_FILLED);
		std::vector<cv::Point>::iterator vertex;
		for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		{
		    cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		}
	    }
	    else if (approxShape.size() == 5)
	    {
		//std::cout << "Area: " << cv::contourArea(approxShape) << std::endl;
		
		drawContours(drawing, contours, i, cv::Scalar(255, 255, 0), CV_FILLED);
		std::vector<cv::Point>::iterator vertex;
		for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		{
		    cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		}
	    }
	    }*/
	
    }

    imshow("Result window", drawing);
}

int main()
{
    //cv::Mat shapeImg = cv::imread("square.jpg");
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
	std::cout << "Video capture is not opened" << std::endl;
	throw cv::Exception();
    }
    
    if (!cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920))
	std::cout << "Resizing failed" << std::endl;
    if (!cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080))
	std::cout << "Resizing failed" << std::endl;
    
    
    while (true)
    {
	cv::Mat shapeImg = GetImage(cap);
	
	//cv::Mat shapeImg = cv::imread("image.jpg");
	shapeImg = ThresholdImage(shapeImg);
	RecognizeShapes(shapeImg);
	
	cv::waitKey(30);
    }

    return 0;
}
