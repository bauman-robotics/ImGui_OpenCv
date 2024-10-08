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

static atomic<double> smoothed_packets_per_second_s(0.0);
atomic<double> smoothed_val_data_per_second_s(0.0);

double CalculateEMA_Socket(double current_value, double previous_ema, double alpha); 
//======================================

void View_Group_Socket_Port_Ctrl(void) {    

    //=== Шестая группа Управление ком-портом =================

    static int combo_current_item = 0;

    ImGui::BeginChild("Socket_Port_Ctrl", ImVec2(L1_P1_SOCKET_PORT_CTRL_W, L1_P1_SOCKET_PORT_CTRL_H), true);

    //========================================
        
    ImGui::Text("Current IP: %d.%d.%d.%d", var.socket.curr_ip[0], var.socket.curr_ip[1], var.socket.curr_ip[2], var.socket.curr_ip[3]);
    
    ImGui::Text("Port:");
    ImGui::SameLine();
    //ImGui::InputInt("##", &var.socket.port);
    ImGui::InputInt("##", &var.socket.port, 0, 0, ImGuiInputTextFlags_CharsDecimal);

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

    #ifdef BINARY_PACKET
        static bool checkbox_hex = true;
    #else 
        static bool checkbox_hex = false;
    #endif 

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

    //========================================

    // Отображение количества пакетов в секунду с использованием EMA
    double alpha = 0.1; // Коэффициент сглаживания
   
    double current_packets_per_second = GetPacketsPerSecond_S();
    smoothed_packets_per_second_s.store(CalculateEMA_Socket(current_packets_per_second, smoothed_packets_per_second_s.load(), alpha));

    // Использование stringstream для форматирования строки
    stringstream ss;
    ss << fixed << setprecision(2) << "Пакетов в секунду: " << setw(6) << smoothed_packets_per_second_s.load();
    ImGui::Text("%s", ss.str().c_str());

    //============  Количество значений в секунду =======
    double current_val_data_per_second = Get_Val_Data_PerSecond_S(); //parsed_data.size();//
    smoothed_val_data_per_second_s.store(CalculateEMA_Socket(current_val_data_per_second, smoothed_val_data_per_second_s.load(), alpha));
    // Использование stringstream для форматирования строки
    stringstream ss1;
    ss1 << fixed << setprecision(2) << "Значений в секунду: " << setw(6) << smoothed_val_data_per_second_s.load();
    ImGui::Text("%s", ss1.str().c_str());


    ImGui::EndChild();
}
//==================================================================================


// Функция для расчета экспоненциального скользящего среднего (EMA)
double CalculateEMA_Socket(double current_value, double previous_ema, double alpha) {
    return alpha * current_value + (1 - alpha) * previous_ema;
}
//==================================================================================
