#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
//#include <opencv2/video/video.hpp>
#include <iostream>
#include <zbar.h>

cv::Mat readFromCamera(int deviceNum) {
    cv::Mat img;
    cv::VideoCapture cap(deviceNum);
    if (!cap.isOpened()) {
        std::cout << "video capture is not opened" << std::endl;
        throw cv::Exception();
    }

    if (!cap.read(img)) {
        std::cout << "Read failed" << std::endl;
        throw cv::Exception();
    }

    //  imshow("original img", img);
    imwrite("img.jpg", img);
    cv::waitKey();

    // convert img to grayscale
    cv::cvtColor(img, img, CV_BGR2GRAY);

    return img;
}

void decodeFrame(cv::Mat frame_grayscale) {

    cv::Mat frame(frame_grayscale);

    // Obtain image data
    int width = frame_grayscale.cols;
    int height = frame_grayscale.rows;
    uchar *raw = (uchar *)(frame_grayscale.data);

    // Wrap image data
    zbar::Image image(width, height, "Y800", raw, width * height);

    // Create a zbar reader
    zbar::ImageScanner scanner;

    // Configure the reader
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    // Scan the image for barcodes
    //int n = scanner.scan(image);
    scanner.scan(image);

    // Extract results
    for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {

        // decode QR
        if (symbol->get_type() == zbar::ZBAR_QRCODE) {
            std::cout << "Decoded QR: \"" << symbol->get_data() << '"' << std::endl;
            std::cout << "Location: (" << symbol->get_location_x(0) << "," << symbol->get_location_y(0) << ")" << std::endl;
            
	    //the number of points in the location polygon
	    std::cout << "Size: " << symbol->get_location_size() << std::endl;

            // Draw location of the symbols found
            if (symbol->get_location_size() == 4) {
                //rectangle(frame, Rect(symbol->get_location_x(i), symbol->get_location_y(i), 10, 10), Scalar(0, 255, 0));
                line(frame, cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(1), symbol->get_location_y(1)), cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(2), symbol->get_location_y(2)), cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Scalar(0, 255, 0), 2, 8, 0);
                line(frame, cv::Point(symbol->get_location_x(3), symbol->get_location_y(3)), cv::Point(symbol->get_location_x(0), symbol->get_location_y(0)), cv::Scalar(0, 255, 0), 2, 8, 0);
            }

	    // Show captured frame, now with overlays!
          imshow("captured", frame);

        }
    }
}

int main(int argc, char **argv) {

    cv::Mat img_gray;
    // Grab the image in which we search for QR codes
    if (argc == 2) {
        // image input
        img_gray = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    } else {
        //img_gray = readFromCamera(0);
        img_gray = cv::imread("pokemon_qr.png", CV_LOAD_IMAGE_GRAYSCALE);
    }

    decodeFrame(img_gray);


    ///////

    cv::OrbFeatureDetector detector(5000);
    std::vector<cv::KeyPoint> img_keypoints, qr_keypoints;
    cv::Mat img_descriptors, qr_descriptors;

    cv::Mat qr = cv::imread("qr_code_one_square2.jpg");

    ///////////////////////////////

    // Apply dynamic thresholding

    cv::Mat img;
    //adaptiveThreshold(InputArray src, OutputArray dst, double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C);
    cv::adaptiveThreshold(img_gray, img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
            cv::THRESH_BINARY, 41, 15);

    // Using static thresholding
    //threshold(src, dst, threshold, maxValue, thresholdType)
    //cv::threshold(img_gray, img, 10, 255, CV_THRESH_BINARY);

    //cv::imshow("grayscale img", img_gray);
    //cv::imshow("img with thresholding", img);

    ///////////////////////////////

    // Find key features in the QR code and the image being processed
    detector(qr, cv::Mat(), qr_keypoints, qr_descriptors);
    drawKeypoints(qr, qr_keypoints, qr);

    detector(img, cv::Mat(), img_keypoints, img_descriptors);
    //drawKeypoints(img, img_keypoints, img, cv::Scalar(255, 0, 0));

    // Identify matching keypoints between QR code and the image
    // i.e. find QR code-like features in the image
    cv::BFMatcher matcher; // brute force matcher
    std::vector<cv::DMatch> matches;
    matcher.match(qr_descriptors, img_descriptors, matches);

    cv::Mat img_matches;
    cv::drawMatches(qr, qr_keypoints, img, img_keypoints, matches, img_matches,
            cv::Scalar(0, 255, 0), cv::Scalar(255, 0, 0), std::vector<char>(),
            cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //for (size_t i = 0; i < keypoints.size(); i++)
    //  cv::circle(img, keypoints[i].pt, 2,
    //      cv::Scalar(0, 0, 255), 1);

    cv::imshow("img_matches", img_matches);

    // find avg location of matching keypoints in the img
    std::vector<cv::Point> matching_keypoints_qr;
    int avg_x = 0;
    int avg_y = 0;
    for (int i = 0; i < matches.size(); i++) {
        // indices of img_keypoints that match a keypoint in the QR identifier
        int img_idx = matches[i].trainIdx;

        cv::Point p1 = img_keypoints[img_idx].pt;
        avg_x += p1.x;
        avg_y += p1.y;

        matching_keypoints_qr.push_back(p1);
    }

    if (!matching_keypoints_qr.empty()) {
        avg_x /= matching_keypoints_qr.size();
        avg_y /= matching_keypoints_qr.size();
    }

    // Circle the avg location of matching keypoints
    std::cout << "x: " << avg_x << " y: " << avg_y << std::endl;
    cv::circle(img, cv::Point(avg_x, avg_y), 10, cv::Scalar(0, 255, 100), 2);

    // Mark center of the image (to compare to the avg location of keypoints)
    cv::circle(img, cv::Point(img.size().width / 2, img.size().height / 2), 10,
            cv::Scalar(255, 100, 0), 2);

    cv::imshow("img", img);
    //cv::imshow("qr", qr);
    cv::waitKey();
}
