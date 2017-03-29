#include "camera.h"

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <zbar.h>
#include <iomanip>

<<<<<<< HEAD
=======

>>>>>>> 81b70bd05aaa2f553951a085d27b7f8155f069aa
cv::Scalar BLUE = cv::Scalar(255, 0, 0);
cv::Scalar GREEN = cv::Scalar(0, 255, 0);
cv::Scalar RED = cv::Scalar(0, 0, 255);
cv::Scalar PURPLE = cv::Scalar(255, 0, 255);
cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
cv::Scalar WHITE = cv::Scalar::all(255);
cv::Scalar BLACK = cv::Scalar::all(0);

<<<<<<< HEAD
=======

>>>>>>> 81b70bd05aaa2f553951a085d27b7f8155f069aa
void drawGrid(cv::Mat mat_img) {

    int stepSize = 50;

    int width = mat_img.size().width;
    int height = mat_img.size().height;

    for (int i = 0; i<height; i += stepSize)
        cv::line(mat_img, cv::Point(0, i), cv::Point(width, i), YELLOW);

    for (int i = 0; i<width; i += stepSize)
        cv::line(mat_img, cv::Point(i, 0), cv::Point(i, height), PURPLE);
}

static double distanceBtwPoints(const cv::Point a, const cv::Point b)
{
    double xDiff = a.x - b.x;
    double yDiff = a.y - b.y;

    return std::sqrt((xDiff * xDiff) + (yDiff * yDiff));
}

void annotate(cv::Mat img, std::string text, cv::Point loc) {
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    double fontScale = 1;
    int thickness = 2;
    cv::putText(img, text, loc, fontFace, fontScale, WHITE, thickness, 8);
}


Camera::Camera(int deviceNum)
{
    cap = cv::VideoCapture(deviceNum);
    if (!cap.isOpened())
    {
        std::cout << "Video capture is not opened" << std::endl;
        throw cv::Exception();
    }

    //if (!cap.set(CV_CAP_PROP_FRAME_WIDTH, 960))
        //std::cout << "Resizing failed" << std::endl;
    //if (!cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720))
        //std::cout << "Resizing failed" << std::endl;
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

    // Configure the QR code reader
    zbar::ImageScanner(scanner);
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

    imgCounter = 0;
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

void Camera::decodeFrame(cv::Mat frame) {

    // Convert to grayscale
    cv::Mat frame_grayscale;
    cvtColor(frame, frame_grayscale, CV_BGR2GRAY);

    // Obtain image data
    int width = frame_grayscale.cols;
    int height = frame_grayscale.rows;
    uchar *raw = (uchar *)(frame_grayscale.data);

    // Wrap image data
    zbar::Image image(width, height, "Y800", raw, width * height);

    // Scan the image for barcodes
    //int n = scanner.scan(image);
    int n = scanner.scan(image);

    if (n > 0) {

        drawGrid(frame);
        // Extract results
        int counter = 0;
        for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            time_t now;
            tm *current;
            now = time(0);
            current = localtime(&now);

            // do something useful with results
            std::cout    << "[" << current->tm_hour << ":" << current->tm_min << ":" << std::setw(2) << std::setfill('0') << current->tm_sec << "] " << counter << " "
                    << "decoded " << symbol->get_type_name()
                    << " symbol \"" << symbol->get_data() << '"' << std::endl;

            std::cout << "Location: (" << symbol->get_location_x(0) << "," << symbol->get_location_y(0) << ")" << std::endl;
            std::cout << "Size: " << symbol->get_location_size() << std::endl;

            // Draw location of the symbols found
            if (symbol->get_location_size() == 4) {
                //rectangle(frame, cv::Rect(symbol->get_location_x(i), symbol->get_location_y(i), 10, 10), cv::Scalar(0, 255, 0));
                line(frame, cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Scalar(0, 255, 0), 2, 8, 0);
            }

            cv::circle(frame, cv::Point(0, 0), 10, BLACK, 2);
            annotate(frame, "(0,0)",  cv::Point(1, 1));
            cv::circle(frame, cv::Point(0, frame.size().height), 10, BLACK, 2);
            cv::circle(frame, cv::Point(frame.size().width, 0), 10, BLACK, 2);
            cv::circle(frame, cv::Point(frame.size().width, frame.size().height), 10, BLACK, 2);

            cv::Point topLeftPoint(symbol->get_location_x(0), symbol->get_location_y(0));
            cv::Point topRightPoint(symbol->get_location_x(1), symbol->get_location_y(1));
            cv::Point bottomLeftPoint(symbol->get_location_x(2), symbol->get_location_y(2));
            cv::Point bottomRightPoint(symbol->get_location_x(3), symbol->get_location_y(3));

            // Mark the edges of the QR code
            //cv::circle(frame, topLeftPoint, 10, RED, 2);
            //cv::circle(frame, topRightPoint, 10, GREEN, 2);
            //cv::circle(frame, bottomLeftPoint, 10, BLUE, 2);
            //cv::circle(frame, bottomRightPoint, 10, YELLOW, 2);

            // Mark the center of the code
            cv::Point2f mid = (topRightPoint+bottomRightPoint)*.5;
            cv::circle(frame, mid, 10, WHITE, 2);
            std::cout << "qr code center: " << mid.x << ", " << mid.y << std::endl;

            // Find the angle of the code from the camera
            cv::Point imageCenter(frame.size().width/2, frame.size().height/2);
            cv::circle(frame, imageCenter, 10, BLACK, 2);
            double distanceFromCenter =  distanceBtwPoints(imageCenter,mid);
            double angleFromCam = distanceFromCenter *.069337525;
            line(frame, imageCenter, mid, cv::Scalar(0, 255, 0), 2, 8, 0);

            std::cout << "distance from center: " << distanceFromCenter << std::endl;
            std::cout << "angle from cam: " << angleFromCam << std::endl;
            std::stringstream stream;
            stream << angleFromCam;
            annotate(frame, stream.str() + "°", cv::Point(imageCenter.x + 20, imageCenter.y));

            counter++;
        }

        // Show captured frame, now with overlays!
        imshow("captured", frame);

        // clean up
        image.set_data(NULL, 0);
        cv::waitKey(30);
    }
}
