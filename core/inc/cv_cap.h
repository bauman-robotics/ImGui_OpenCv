
#pragma once

#include "main.h"

#ifdef USE_OPENCV
    void showImage(GLuint texture, const cv::Mat& img);
    int Cv_Processing(cv::VideoCapture & cap, GLuint * texture); 
#endif 
