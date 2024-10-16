#include "main.h"
#include "defines.h"
#include "win_defines.h"

void Menu_Item(void) {
    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(MAIN_MENU_POS_W, MAIN_MENU_POS_H), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(MAIN_MENU_W, MAIN_MENU_H), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Menu Window", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    // Добавляем кнопки управления
    ImGui::PushStyleColor(ImGuiCol_Button, var.com_port_mode ? activeColor : defaultColor);
    if (ImGui::Button("                         Com-port                           ")) {

        sprintf(var.socket.send.message, "%s", "Serial");          
        var.socket.send.need_to_be_sended.store(1); 

        Select_Mode(COM_PORT_MODE);  
        InitSerial(); 
        Close_CV();    
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, var.ctrl_mode ? activeColor : defaultColor);
    if (ImGui::Button("                      Socket-Server                         ")) {
        
        sprintf(var.socket.send.message, "%s", "Wifi");          
        var.socket.send.need_to_be_sended.store(1); 

        Select_Mode(CTRL_MODE);  
        CloseSerial();
        Close_CV();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, var.post_request_mode ? activeColor : defaultColor);
    if (ImGui::Button("    Post Request   ")) {

        sprintf(var.socket.send.message, "%s", "Post");          
        var.socket.send.need_to_be_sended.store(1);         

        Select_Mode(POST_REQUEST_MODE); 
        CloseSerial();

    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, var.cv_mode ? activeColor : defaultColor);
    //if (ImGui::Button("                         OpenCv                           ")) {    
    if (ImGui::Button("    OpenCv    ")) {

        Select_Mode(OPENCV_MODE); 
        CloseSerial();
        if (!var.Init_CV_done) {
            Init_CV();
        }
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    // Использование stringstream для форматирования строки
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "Cpu Load: " << std::setw(6) << var.cpu_load << " %";
    ImGui::Text("%s", ss.str().c_str());



    ImGui::End();
}