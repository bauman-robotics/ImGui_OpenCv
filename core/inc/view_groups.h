#pragma once

void View_Group_Test_Ctrls();
void View_Group_Socket_Data_Ctrl();
void View_Group_Mouse_Ctrl();
void View_Group_Mouse_Plot();
void View_Group_Serial_Plot(); // Serial data
void View_Group_Serial_Ctrl(); // Serial ctrl

void View_Group_Socket_Plot(); // Socket data
void View_Group_Socket_Port_Ctrl(); // Socket ctrl

int View_Cv_Group_1(cv::VideoCapture & cap);
void View_Cv_Group_2();

void Clear_Plot_Serial_Data(); 
void Clear_Socket_Data();
void View_Group_9_Socket_Logs();