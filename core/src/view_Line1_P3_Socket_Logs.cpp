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
#include "defines.h"
#include "win_defines.h"

using namespace std;
namespace fs = filesystem;


//======================================

void View_Group_9_Socket_Logs() {    

    ImGui::BeginChild("Socket_Logs", ImVec2(L1_P3_SOCKET_LOGS_CTRL_W, L1_P3_SOCKET_LOGS_CTRL_H), true);

    ImGui::Text("Logs");

    static int combo_current_item = 0;
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию


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


    if (ImGui::Button("Open File")) {

        cout <<  "Open Lig file: " << var.log.curr_Log_File_Name << endl;


        // Open log file 
        string command = "xdg-open " + var.log.curr_Log_File_Name;
        
        int result = system(command.c_str());
        if (result != 0) {
            cerr << "Failed to open folder: " << var.log.curr_Log_File_Name << endl;
        }    
    }
    //=======================================
    ImGui::SameLine();

    if (ImGui::Button("Open Folder")) {

        cout <<  "Open Folder: " << var.log.currentFolderName << endl;

        Open_Folder(var.log.currentFolderName);  
    }

    //ImGui::SameLine();

    //========================================
    if (ImGui::Button("Clear Folder")) {

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
