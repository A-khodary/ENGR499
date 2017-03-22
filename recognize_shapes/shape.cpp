#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Mat GetImage()
{
    // Read shape image
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
    
    // Take image
    cv::Mat shapeImg;
    if (!cap.read(shapeImg))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }

    imwrite("image.jpg", shapeImg);
    imshow("shapeImg", shapeImg);

    return shapeImg;
}

cv::Mat ThresholdImage(cv::Mat shapeImg)
{
    // Convert to binary
    cv::cvtColor(shapeImg, shapeImg, CV_BGR2GRAY);
    
    cv::adaptiveThreshold(shapeImg, shapeImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 21, 30);
    
    //cv::threshold(shapeImg, shapeImg, 50, 255, CV_THRESH_BINARY);

    imshow("shapeImg", shapeImg);

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
    findContours(shapeImg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    
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

    std::vector<cv::Point> approxSquare;
    for (int i = 0; i < contours.size(); i++)
    {
	approxPolyDP(contours[i], approxSquare, arcLength(cv::Mat(contours[i]), true) * 0.05, true);
	if (approxSquare.size() == 4)
	{
	    drawContours(drawing, contours, i, cv::Scalar(0, 255, 255), CV_FILLED);
	    std::vector<cv::Point>::iterator vertex;
	    for (vertex = approxSquare.begin(); vertex != approxSquare.end(); vertex++)
	    {
		cv::circle(drawing, *vertex, 3, cv::Scalar(0, 0, 255), 1);
	    }
	}
    }

    imshow("Result window", drawing);
}

int main()
{
    cv::Mat shapeImg = GetImage();

    //cv::Mat shapeImg = cv::imread("image.jpg");
    shapeImg = ThresholdImage(shapeImg);
    RecognizeShapes(shapeImg);
    
    cv::waitKey();

    return 0;
}
