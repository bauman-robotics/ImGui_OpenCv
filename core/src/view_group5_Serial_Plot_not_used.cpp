


// not used now 


#include "main.h"
#include "uart.h"
#include <atomic>
#include "win_defines.h"
#include "defines.h"

static std::atomic<double> smoothed_packets_per_second(0.0);
static std::atomic<double> smoothed_val_data_per_second(0.0);
static std::vector<float> y_coords;

static std::mutex data_mutex;
static std::vector<int> parsed_data;
static bool freeze_en = 0;

double CalculateEMA(double current_value, double previous_ema, double alpha);
void Clear_Plot_Serial_Data();
static void Pars_Data_And_Binary_Log(); 

extern vector<float> parseBinarySocketData_Float();
//======================================

void View_Group_Serial_Plot(void) {    

    //=== Пятая группа графиком данных из последовательного порта работающего в отдельном потоке =================
    
    ImGui::BeginChild("Serial_Plot", ImVec2(SERIAL_PLOT_W, SERIAL_PLOT_H), true);
    ImGui::Text("Данные из последовательного порта");

    if (!freeze_en) {
        Pars_Data_And_Binary_Log();
    }
    //====================================================
    // std::vector<int> new_parser_data;
    // // Парсинг данных
    // if (var.com_port.init_serial_done) {
    //     new_parser_data = parseComPortData_Float(DATA_PREFIX);

    //     parsed_data.insert(parsed_data.end(),
    //             new_parser_data.begin(),
    //             new_parser_data.end());
    // }
    //====  График значений ===========

    // Преобразование данных в формат для ImGui::PlotLines
    // y_coords.clear();
    // for (int number : parsed_data) {
    //     y_coords.push_back(static_cast<float>(number));
    // }

    // Радиокнопки для выбора типа графика
    static int graph_type = 0; // 0 - линии, 1 - гистограмма, 2 - окно последних значений
    ImGui::RadioButton("Линии", &graph_type, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Гистограмма", &graph_type, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Окно последних значений", &graph_type, 2);

    //==================================================================================
    // Слайдер для регулировки ширины окна последних значений
    static int window_size = 100; // Размер окна последних значений
    if (graph_type == 2) {
        ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
    } else {
        ImGui::BeginDisabled();
        ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
        ImGui::EndDisabled();
    }

    // Получаем доступную ширину контента
    ImVec2 available_size = ImGui::GetContentRegionAvail();

        // Устанавливаем желаемую высоту графика
    float plot_height = 150.0f;


    // Отображение графика
    if (graph_type == 0) {
        // Рисуем график, используя доступную ширину и заданную высоту
        ImGui::PlotLines("Serial", var.socket.data_f.data(), var.socket.data_f.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(available_size.x, plot_height));


        //ImGui::PlotLines("Serial", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 1) {
        ImGui::PlotHistogram("Serial", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 2) {
        if (y_coords.size() > window_size) {
            ImGui::PlotLines("Serial", &y_coords[y_coords.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        } else {
            ImGui::PlotLines("Serial", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        }
    }

    // Кнопка для очистки графика
    if (ImGui::Button("Очистить график")) { 
        Clear_Plot_Serial_Data();      
    }
    //================================

    ImGui::SameLine();

    // Отображение количества пакетов в секунду с использованием EMA
    double alpha = 0.1; // Коэффициент сглаживания
   
     double current_packets_per_second = GetPacketsPerSecond();
    smoothed_packets_per_second.store(CalculateEMA(current_packets_per_second, smoothed_packets_per_second.load(), alpha));

    // Использование stringstream для форматирования строки
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "Пакетов в секунду: " << std::setw(6) << smoothed_packets_per_second.load();
    ImGui::Text("%s", ss.str().c_str());

    //============  Количество значений в секунду =======
    double current_val_data_per_second = Get_Val_Data_PerSecond(); //parsed_data.size();//
    smoothed_val_data_per_second.store(CalculateEMA(current_val_data_per_second, smoothed_val_data_per_second.load(), alpha));
    // Использование stringstream для форматирования строки
    std::stringstream ss1;
    ss1 << std::fixed << std::setprecision(2) << "Значений в секунду: " << std::setw(6) << smoothed_val_data_per_second.load();
    ImGui::Text("%s", ss1.str().c_str());

    //======================================================

    ImGui::EndChild();
}
//==================================================================================

// Функция для расчета экспоненциального скользящего среднего (EMA)
double CalculateEMA(double current_value, double previous_ema, double alpha) {
    return alpha * current_value + (1 - alpha) * previous_ema;
}
//==================================================================================

void Clear_Plot_Serial_Data() {
    parsed_data.clear(); 
    y_coords.clear();   
}
//==================================================================================

static void Pars_Data_And_Binary_Log() {

    // Парсинг данных
    // if (var.socket.init_socket_done) {

        //if (!var.socket.hex_receive) {

            y_coords = parseComPortData_Float(var.socket.data_prefix);
            #ifdef LOG_ONE_VAL_TO_LINE
                if (var.log.log_Is_Started) {
                    if (y_coords.size()) {                    
                    
                        //parseBinarySocketData_Float();
                    }
                } 
            #endif 

        //}  else {
        //     // y_coords - only for logs 
        //     //==== output ====
        //     // var.socket.data_f 
        //     //y_coords =  parseBinarySocketData_Float();  

        //     if (var.log.log_Is_Started) {
        //         if (y_coords.size()) {                    
                    
        //             Add_Str_To_Log_File_HEX_Float(y_coords);
        //         }
        //     } 
        //}               
    // }
} 