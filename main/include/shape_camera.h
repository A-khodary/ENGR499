// Class for analyzing images for shapes
#ifndef SHAPE_CAMERA
#define SHAPE_CAMERA

#include "camera.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class ShapeCamera
{
 public:
    ShapeCamera(Camera* camera);
    cv::Mat TakePicture();
    cv::Mat ThresholdImage(cv::Mat shapeImg);
    void RecognizeShapes(cv::Mat shapeImg);

 private:
    Camera* camera;
};

#endif
