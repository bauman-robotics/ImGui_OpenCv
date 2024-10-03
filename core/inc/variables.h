#pragma once
//==================
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
//==================
#include <opencv2/opencv.hpp>
//============================
#include <atomic>
//================================
using namespace std;

typedef struct {
    uint8_t low;       // Тип сообщения
    uint8_t hi;    
    uint16_t val;
} Header_Type;

typedef struct {
    Header_Type type;               // Тип сообщения
    Header_Type full_packet_size;   // Размер данных в байтах
} BinPacketHeader;


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
    string name;
    string last_port_name;
    string data_prefix;
    uint32_t i_baud_rate;
    //==================
    vector<const char*> ports_list;
    string selected_port;
    int selected_port_index;
    bool init_serial_done;
    bool have_to_be_closed;
    //==================    
} com_port_s;
//================================
typedef struct {  
    //==================     
    char message[10];
    atomic<bool> need_to_be_sended;  
    //==================    
} socket_send;
//================================
typedef struct {  
    //==================     
    string data_prefix;
    //==================
    uint8_t curr_ip[4];
    int port;
    bool init_socket_done;
    bool have_to_be_closed;
    bool have_to_be_binded;    
    //bool have_to_be_open;    
    //==================    
    socket_send send;
    bool hex_receive;
    bool chart_enable;
    vector<float> data_f;
} socket_s;
//================================

typedef struct {
    string curr_Log_File_Name;
    bool    log_Is_Started;
    string currentFolderName;
    string strFullFileName;
} Log_File_Type;
//================================

typedef struct {
    bool mouse_chart_enable;
    int groupSize_x;
    int groupSize_y;
} Mouse_Type;
//================================

// typedef struct {
//     atomic<int32_t> parser_data_size;
//     atomic<int32_t> new_parser_data_size;
//     atomic<int32_t> plot_data_size;
// } Debug_Type;


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
    Mouse_Type mouse;
  //  Debug_Type debug;

} variables;



extern variables var;

//============================
