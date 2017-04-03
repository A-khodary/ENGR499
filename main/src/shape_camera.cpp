#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iomanip>

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
    
    //cv::adaptiveThreshold(shapeImg, shapeImg, 255,
    //			  cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 51, 15);
    
    cv::adaptiveThreshold(shapeImg, shapeImg, 255,
    			  cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 121, 15);

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
    int pos3 = -1, pos4 = -1, pos5 = -1;
    for (int i = 0; i < contours.size(); i++)
    {	
      	approxPolyDP(contours[i], approxShape,
		     arcLength(cv::Mat(contours[i]), true) * 0.05, true);

	if (approxShape.size() >= 3 && cv::contourArea(approxShape) > 100
	    && cv::contourArea(approxShape) < 5000)
	{
	    if (approxShape.size() == 3)
	    {
		double perimeter = cv::arcLength(approxShape, true);
		double sideLength = perimeter / 3;
		double approxArea = 1.73205 / 4 * pow(sideLength, 2);

		double ratio = approxArea / cv::contourArea(approxShape);
		if (ratio > 0.9 && ratio <  1.1)
		{
		    drawContours(drawing, contours, i, cv::Scalar(255, 0, 255), CV_FILLED);
		    std::vector<cv::Point>::iterator vertex;
		    for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		    {
			cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		    }

		    // Get the position
		    pos3 = (approxShape[0].x + approxShape[1].x +
			    approxShape[2].x) / 3;
		}
	    }
	    else if (approxShape.size() == 4)
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

		    // Get the position
		    pos4 = (approxShape[0].x + approxShape[1].x +
			    approxShape[2].x + approxShape[3].x) / 4;
		}
	    }
	    else if (approxShape.size() == 5)
	    {
		double perimeter = cv::arcLength(approxShape, true);
		double sideLength = perimeter / 5;
		double approxArea = 0.25 * 6.8819 * pow(sideLength, 2);

		double ratio = approxArea / cv::contourArea(approxShape);
		if (ratio > 0.95 && ratio <  1.05)
		{
		    drawContours(drawing, contours, i, cv::Scalar(255, 255, 0), CV_FILLED);
		    std::vector<cv::Point>::iterator vertex;
		    for (vertex = approxShape.begin(); vertex != approxShape.end(); vertex++)
		    {
			cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
		    }

		    // Get the position
		    pos5 = (approxShape[0].x + approxShape[1].x +
			    approxShape[2].x + approxShape[3].x +
			    approxShape[4].x) / 5;
		}
	    }
	}	
    }

    
    // Detect angle of shape
    double angle3 = (double) (pos3 - 340) / 340 * 37.5;
    double angle4 = (double) (pos4 - 340) / 340 * 37.5;
    double angle5 = (double) (pos5 - 340) / 340 * 37.5;
    
    std::string string3, string4, string5;
    if (pos3 == -1)
    {
	string3 = "X";
    }
    else
    {
	string3 = std::to_string(angle3);
    }
    if (pos4 == -1)
    {
	string4 = "X";
    }
    else
    {
	string4 = std::to_string(angle4);
    }
    if (pos5 == -1)
    {
	string5 = "X";
    }
    else
    {
	string5 = std::to_string(angle5);
    }
    
    std::cout << "angle3:" << std::setw(13) << string3 << ", angle4:"
	      << std::setw(13) << string4 << ", angle5:"
	      << std::setw(13) << string5 << '\r';
    
    
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
    
    //std::cout << std::endl;
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
