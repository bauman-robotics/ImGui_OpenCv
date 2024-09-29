#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>

#include <tcp-Server.h>
#include <view_groups.h>
//======================================



//======================================

void View_Group_8(void) {    

    //=== Шестая группа Управление ком-портом =================

    static int combo_current_item = 0;

    ImGui::BeginChild("Group 8", ImVec2(280, 270), true);

    ImGui::Text("Socket port");
    ImGui::InputInt("##", &var.socket.port);

    //========================================

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    ImGui::PushStyleColor(ImGuiCol_Button, var.socket.init_socket_done ? activeColor : defaultColor);

    if (ImGui::Button("       Open       ")) {
        Clear_Socket_Data();
        Socket_Server_Init(var.socket.port);

        //var.socket.have_to_be_open = 1;
        
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();

    //========================================
    if (ImGui::Button("       Close       ")) {
        if (var.socket.init_socket_done) {
            Socket_Close();

            //var.socket.have_to_be_open = 0;
        }
    }
    //========================================
    ImGui::PushStyleColor(ImGuiCol_Button, var.log.log_Is_Started ? activeColor : defaultColor);

    if (ImGui::Button("  Start Save   ")) {
        Create_Log_File();
        //var.log.log_Is_Started = 1;
        
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();

    //========================================
  
    if (ImGui::Button("   Stop Save    ")) {
        var.log.log_Is_Started = 0;
    }
 
    ImGui::EndChild();
}
//==================================================================================

