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
    std::string last_port_name;
    std::string data_prefix;
    uint32_t i_baud_rate;
    //==================
    std::vector<const char*> ports_list;
    std::string selected_port;
    int selected_port_index;
    bool init_serial_done;
    bool have_to_be_closed;
    //==================    
} com_port_s;
//================================
typedef struct {  
    //==================     
    std::string data_prefix;
    //==================
    int port;
    bool init_socket_done;
    bool have_to_be_closed;
    bool have_to_be_binded;    
    //bool have_to_be_open;    
    //==================    
} socket_s;
//================================

typedef struct {
    std::string curr_Log_File_Name;
    bool    log_Is_Started;
    std::string currentFolderName;
    std::string strFullFileName;
} Log_File_Type;
//================================

typedef struct {   
    bool cv_mode;
    bool ctrl_mode;
    bool com_port_mode;
    //==================
    bool Init_CV_done;
    bool init_all_done;

    //==================    
    cv_ctrl cv;
    //==================
    char iniFileName[255];
    ImGuiIO* io;
    com_port_s com_port;
    socket_s socket;
    //==================    
    double cpu_load;
    //==================
    Log_File_Type log;

} variables;

extern variables var;

//============================
