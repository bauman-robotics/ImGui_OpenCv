#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>

#include <tcp-Server.h>
//======================================



//======================================

void View_Group_8(void) {    

    //=== Шестая группа Управление ком-портом =================

    static int combo_current_item = 0;

    ImGui::BeginChild("Group 8", ImVec2(280, 270), true);

    // // Установка ширины комбобокса
    // ImGui::SetNextItemWidth(150.0f); // Установите желаемую ширину комбобокса

    // // Отображение комбобокса с портами
    // if (!var.com_port.ports_list.empty()) {
    //     ImGui::Combo("Порт", &combo_current_item, var.com_port.ports_list.data(), var.com_port.ports_list.size());
    //     // Сохранение выбранного элемента в переменную
    //     var.com_port.selected_port = var.com_port.ports_list[combo_current_item];
    
    // } else {
    //     ImGui::Text("Нет портов");
    //     var.com_port.selected_port = "-1";
    // }

    static char buf_port[80] = {0};
    ImGui::Text("Socket port = %d", var.socket.port);

    //========================================

    // if (ImGui::Button("    Review    ")) {
    //     var.com_port.ports_list = getConnectedTTYACMPorts();
    // }

    //========================================
    //ImGui::SameLine();

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    ImGui::PushStyleColor(ImGuiCol_Button, var.socket.init_socket_done ? activeColor : defaultColor);

    if (ImGui::Button("        Open        ")) {

        Socket_Server_Init(var.socket.port);
        
    }
    ImGui::PopStyleColor();
    //========================================
    if (ImGui::Button("                    Close                     ")) {
        if (var.socket.init_socket_done) {
            Socket_Close();
            //Socket_Stop_Read();
        }
    }
    //========================================
    ImGui::EndChild();
}
//==================================================================================

