/*
 * Main file for starting a camera thread
 */
#include "camera.h"

int main()
{
    Camera camera(0);
    camera.TakePicture();

    return 0;
}
