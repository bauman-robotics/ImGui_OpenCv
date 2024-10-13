#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>
#include "win_defines.h"
//======================================



//======================================

void View_Group_Serial_Ctrl(void) {    

    //=== Шестая группа Управление ком-портом =================

    static int combo_current_item = 0;

    ImGui::BeginChild("Group 6", ImVec2(SERIAL_CTRL_W, SERIAL_CTRL_H), true);

    // Установка ширины комбобокса
    ImGui::SetNextItemWidth(150.0f); // Установите желаемую ширину комбобокса

    // Отображение комбобокса с портами
    if (!var.com_port.ports_list.empty()) {
        ImGui::Combo("Порт", &combo_current_item, var.com_port.ports_list.data(), var.com_port.ports_list.size());
        // Сохранение выбранного элемента в переменную
        var.com_port.selected_port = var.com_port.ports_list[combo_current_item];
    
    } else {
        ImGui::Text("Нет портов");
        var.com_port.selected_port = "-1";
    }

    //========================================

    if (ImGui::Button("    Review    ")) {
        var.com_port.ports_list = getConnectedTTYACMPorts();
    }

    //========================================
    ImGui::SameLine();

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    ImGui::PushStyleColor(ImGuiCol_Button, var.com_port.init_serial_done ? activeColor : defaultColor);

    if (ImGui::Button("        Open        ")) {
        if (!var.com_port.init_serial_done) {

            InitSerial();

        }        
        
    }

    //=== Temp =====    
    if (!var.log.log_Is_Started) {

        Create_Log_File();  // temp 
    }

    ImGui::PopStyleColor();
    //========================================
    if (ImGui::Button("                    Close                     ")) {
        if (var.com_port.init_serial_done) {
            CloseSerial();
        }
    }
    //========================================
    ImGui::EndChild();
}
//==================================================================================


//Create_Log_File();  // temp 