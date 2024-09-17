#pragma once
//==================
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
//==================
#include <opencv2/opencv.hpp>
//============================

typedef struct {  
    //==================     
    bool playing;
    bool recording;
    cv::Mat lastFrame;
    cv::VideoWriter videoWriter;
    cv::Mat frame;
    cv::VideoCapture cap;
    GLuint texture;
    int frame_size;
    //==================
} cv_ctrl;
//================================

typedef struct {  
    //==================     
    std::string name;
    std::string data_prefix;
    uint32_t i_baud_rate;
    //==================
} com_port_s;
//================================

typedef struct {   
    bool cv_mode;
    bool ctrl_mode;
    bool com_port_mode;
    //==================
    bool Init_CV_done;
    bool init_all_done;
    bool init_serial_done;
    //==================    
    cv_ctrl cv;
    //==================
    char iniFileName[255];
    ImGuiIO* io;
    com_port_s com_port;

} variables;

extern variables var;

//============================
