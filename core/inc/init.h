#pragma once

#include <opencv2/opencv.hpp>
#include "cv_cap.h"

int Init_All(int * serial_fd, GLFWwindow** window); 

int Init_All_CV(cv::VideoCapture& cap, GLuint * texture, GLFWwindow** window);