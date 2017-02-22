#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
//#include <opencv2/video/video.hpp>
#include <iostream>

int main()
{
    cv::OrbFeatureDetector detector(5000);
    std::vector<cv::KeyPoint> img_keypoints, qr_keypoints;
    cv::Mat img_descriptors, qr_descriptors;

    cv::Mat qr = cv::imread("qr_code_one_square2.jpg");

    //cv::Mat img = cv::imread("yoshi_qr.jpg");



    ////////////////////////
    cv::Mat img_gray = cv::imread("yoshi_qr.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    
    cv::Mat img;
    cv::threshold(img_gray, img, 10, 255, CV_THRESH_BINARY);


    ///////////////////////////////

    detector(qr, cv::Mat(), qr_keypoints, qr_descriptors);
    drawKeypoints(qr, qr_keypoints, qr);


    detector(img, cv::Mat(), img_keypoints, img_descriptors);
    drawKeypoints(img, img_keypoints, img);

    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(qr_descriptors, img_descriptors, matches);

    cv::Mat img_matches;
    cv::drawMatches(qr, qr_keypoints, img, img_keypoints, matches, img_matches);

    //for (size_t i = 0; i < keypoints.size(); i++)
    //    cv::circle(img, keypoints[i].pt, 2,
    //        cv::Scalar(0, 0, 255), 1);

    cv::imshow("img_matches", img_matches);

    std::cout << "x: " << img_keypoints[0].pt.x << " y: " << img_keypoints[0].pt.y << std::endl;

    //cv::imshow("img", img);
    //cv::imshow("qr", qr);
    cv::waitKey();
}