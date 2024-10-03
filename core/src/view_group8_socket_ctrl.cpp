#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>

#include <tcp-Server.h>
#include <view_groups.h>

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include "log_file.h"

using namespace std;
namespace fs = filesystem;

//======================================

void View_Group_8(void) {    

    //=== Шестая группа Управление ком-портом =================

    static int combo_current_item = 0;

    ImGui::BeginChild("Group 8", ImVec2(280, 270), true);

    //========================================
        
    ImGui::Text("Current IP: %d.%d.%d.%d", var.socket.curr_ip[0], var.socket.curr_ip[1], var.socket.curr_ip[2], var.socket.curr_ip[3]);
    
    ImGui::Text("Socket port:");
    ImGui::InputInt("##", &var.socket.port);

    //========================================

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    ImGui::PushStyleColor(ImGuiCol_Button, var.socket.init_socket_done ? activeColor : defaultColor);

    if (ImGui::Button("       Open       ")) {
        Clear_Socket_Data();
        Socket_Server_Init(var.socket.port);
        
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();

    //========================================
    if (ImGui::Button("       Close       ")) {
        if (var.socket.init_socket_done) {
            Socket_Close();
        }
    }
    //========================================
    ImGui::PushStyleColor(ImGuiCol_Button, var.log.log_Is_Started ? activeColor : defaultColor);

    if (ImGui::Button("  Start Save   ")) {
        Create_Log_File();
        cout << "Start Log File" << endl;
        
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();

    //========================================
  
    if (ImGui::Button("   Stop Save    ")) {
        var.log.log_Is_Started = 0;
        cout << "Stop Log File" << endl;
    }
 
    static bool checkbox_hex = false;
    ImGui::Checkbox("HEX Format", &checkbox_hex);

    if (ImGui::IsItemEdited())
    {
        if (checkbox_hex) {
            sprintf(var.socket.send.message, "%s", "HEX");      
            var.socket.hex_receive = 1;    
            var.socket.send.need_to_be_sended.store(1);    
        } else {
            sprintf(var.socket.send.message, "%s", "ASCII");          
            var.socket.hex_receive = 0;
            var.socket.send.need_to_be_sended.store(1); 
        }
    }

    //=======================================

    if (ImGui::Button("  Open Folder  ")) {

        cout <<  "Open Folder: " << var.log.currentFolderName << endl;

        Open_Folder(var.log.currentFolderName);  
    }

    ImGui::SameLine();

    //========================================
    if (ImGui::Button("  Clear Folder  ")) {

        cout <<  "Delete Folder: " << var.log.currentFolderName << endl;
        try {
            for (const auto& entry : fs::recursive_directory_iterator(var.log.currentFolderName)) {                
                if (fs::is_regular_file(entry.path())) {
                    fs::remove(entry.path());
                }
            }
        } catch (const fs::filesystem_error& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
    //========================================

    if (ImGui::Button("    Open File   ")) {

        cout <<  "Open Lig file: " << var.log.curr_Log_File_Name << endl;


        // Open log file 
        string command = "xdg-open " + var.log.curr_Log_File_Name;
        
        int result = system(command.c_str());
        if (result != 0) {
            cerr << "Failed to open folder: " << var.log.curr_Log_File_Name << endl;
        }    
    }

   //========================================
   static bool checkbox = true;
    ImGui::Checkbox("Chart Enable", &checkbox);

    if (ImGui::IsItemEdited())
    {
        if (checkbox) {
            var.socket.chart_enable = 1;
 
            std::cout << "chart_enable" << std::endl;            
        } else {
            var.socket.chart_enable = 0;
            std::cout << "chart_disable" << std::endl;
        }        
    }


    ImGui::EndChild();
}
//==================================================================================

