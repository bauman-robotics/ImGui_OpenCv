#pragma once

#include <opencv2/opencv.hpp>
#include "cv_cap.h"

int Init_All(GLFWwindow** window); 

int Init_CV();

void Close_CV(); 

void Select_Mode(int mode);
int Get_Mode(); 

void InitFrameSize(int size); 