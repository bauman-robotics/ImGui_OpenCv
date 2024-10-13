#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>
#include "win_defines.h"
//======================================

static std::atomic<double> smoothed_packets_per_second(0.0);
static std::atomic<double> smoothed_val_data_per_second(0.0);
static double CalculateEMA(double current_value, double previous_ema, double alpha);
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

    //=== ===========================================================================Temp =====    
    if (!var.log.log_Is_Started) {

        Create_Log_File();  // temp 
    }
    //=================================================================================

    ImGui::PopStyleColor();
    //========================================
    if (ImGui::Button("                    Close                     ")) {
        if (var.com_port.init_serial_done) {
            CloseSerial();
        }
    }


    //========================================

    // #ifdef BINARY_PACKET
    //     static bool checkbox_hex = true;
    // #else 
    //     static bool checkbox_hex = false;
    // #endif 

    static bool checkbox_hex;

    if (var.socket.hex_receive) {
       checkbox_hex = true;
    } else {
        checkbox_hex = false;
    }

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
   
    double current_packets_per_second = GetPacketsPerSecond();
    smoothed_packets_per_second.store(CalculateEMA(current_packets_per_second, smoothed_packets_per_second.load(), alpha));

    cout << "current_packets_per_second " << current_packets_per_second << endl;
    // Использование stringstream для форматирования строки
    stringstream ss;
    ss << fixed << setprecision(2) << "Пакетов в секунду: " << setw(6) << smoothed_packets_per_second.load();
    ImGui::Text("%s", ss.str().c_str());

    //============  Количество значений в секунду =======
    double current_val_data_per_second = Get_Val_Data_PerSecond(); //parsed_data.size();//

    smoothed_val_data_per_second.store(CalculateEMA(current_val_data_per_second, smoothed_val_data_per_second.load(), alpha));
    // Использование stringstream для форматирования строки
    stringstream ss1;
    ss1 << fixed << setprecision(2) << "Значений в секунду: " << setw(6) << smoothed_val_data_per_second.load();
    ImGui::Text("%s", ss1.str().c_str());

    ImGui::EndChild();
}
//==================================================================================

// Функция для расчета экспоненциального скользящего среднего (EMA)
double CalculateEMA(double current_value, double previous_ema, double alpha) {
    return alpha * current_value + (1 - alpha) * previous_ema;
}
//==================================================================================



//Create_Log_File();  // temp 