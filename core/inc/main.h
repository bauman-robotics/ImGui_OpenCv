#pragma once
//==================
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
//==================
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <algorithm>
//==================
#include "defines.h"
#include "fonts.h"
#include "init.h"
//==================

#ifdef USE_OPENCV
    #include <opencv2/opencv.hpp>
    #include "cv_cap.h"
#else 
    #include <view.h>
    #include <fonts.h>
#endif 

#ifdef USE_COM_PORT

#endif 


#include <termios.h>
#include <uart.h>
