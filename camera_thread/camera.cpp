#include "camera.h"

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

Camera::Camera(int deviceNum, std::string QRImage)
{
    cap = cv::VideoCapture(deviceNum);
    //cap = cv::VideoCapture("gst-launch-1.0 nvcamerasrc ! 'video/x-raw(memory:NVMM), width=(int)1920, height=(int)1080, format=(string)I420, framerate=(fraction)30/1' ! nvtee ! nvoverlaysink -e");
    if (!cap.isOpened())
    {
	std::cout << "Video capture is not opened" << std::endl;
	throw cv::Exception();
    }

    if (!cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920))
	std::cout << "Resizing failed" << std::endl;
    if (!cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080))
	std::cout << "Resizing failed" << std::endl;
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    
    imgCounter = 0;
    filePrefix = "images/img";
    fileSuffix = ".jpg";

    qrImage = cv::imread(QRImage);
}

cv::Mat Camera::TakePicture()
{
    cv::Mat img;
    if (!cap.read(img))
    {
	std::cout << "Camera image read failed" << std::endl;
	throw cv::Exception();
    }
    
    std::string filename = filePrefix + std::to_string(imgCounter++)
	+ fileSuffix;
    imshow("image", img);
    //imwrite(filename, img);

    return img;
}

cv::Mat Camera::ConvertToBinary(cv::Mat img)
{
    cv::cvtColor(img, img, CV_BGR2GRAY);

    cv::adaptiveThreshold(img, img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
            cv::THRESH_BINARY, 41, 15);

    cv::cvtColor(img, img, CV_GRAY2BGR);
    
    imshow("binary", img);

    return img;
}

std::pair<int, int> Camera::FindQRCodeCenter(cv::Mat binaryImg)
{
    // Find key features in the QR code and the image being processed
    cv::OrbFeatureDetector detector(5000);
    std::vector<cv::KeyPoint> imgKeypoints, qrKeypoints;
    cv::Mat imgDescriptors, qrDescriptors;

    detector(qrImage, cv::Mat(), qrKeypoints, qrDescriptors);
    drawKeypoints(qrImage, qrKeypoints, qrImage);
    detector(binaryImg, cv::Mat(), imgKeypoints, imgDescriptors);

    // Identify matchings keypoints between QR code and the image
    // i.e. find QR code-like features in the image
    cv::BFMatcher matcher; // brute force matcher
    std::vector<cv::DMatch> matches;
    matcher.match(qrDescriptors, imgDescriptors, matches);

    cv::Mat imgMatches;
    cv::drawMatches(qrImage, qrKeypoints, binaryImg, imgKeypoints, matches,
		    imgMatches, cv::Scalar(0, 255, 0), cv::Scalar(255, 0, 0),
		    std::vector<char>(),
		    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    cv::imshow("imgMatches", imgMatches);

    // Find average location of matching keypoints in the img
    std::vector<cv::Point> matchingKeypointsQR;
    int avgX = 0;
    int avgY = 0;
    for (int i = 0; i < matches.size(); i++)
    {
	// indices of imgKeypoints that match a keypoint in the
	// QR identifier
	int imgIdx = matches[i].trainIdx;

	cv::Point p1 = imgKeypoints[imgIdx].pt;
	avgX += p1.x;
	avgY += p1.y;

	matchingKeypointsQR.push_back(p1);
    }

    if (!matchingKeypointsQR.empty())
    {
	avgX /= matchingKeypointsQR.size();
	avgY /= matchingKeypointsQR.size();
    }

    // Circle the average location of matching keypoints
    std::cout << "x: " << avgX << " y: " << avgY << std::endl;
    cv::circle(binaryImg, cv::Point(avgX, avgY), 10,
	       cv::Scalar(0, 0, 255), 2);

    // Mark center of the image to compare to the avg location of keypoints
    cv::circle(binaryImg, cv::Point(binaryImg.size().width / 2,
				    binaryImg.size().height / 2),
	       10, cv::Scalar(255, 0, 0), 2);

    cv::imshow("QR Location", binaryImg);
    
    return std::pair<int, int>(avgX, avgY);
}
