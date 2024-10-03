#include "main.h"
#include "uart.h"
#include "tcp-Server.h"
#include <atomic>

#include <iostream>
#include <vector>
#include <string>
#include "defines.h"

#include <deque>

static atomic<double> smoothed_packets_per_second(0.0);
static atomic<double> smoothed_val_data_per_second(0.0);
static vector<float> y_coords;

static mutex data_mutex;
static vector<int> parsed_data;   

using namespace std;

double CalculateEMA_Socet(double current_value, double previous_ema, double alpha);
std::vector<float> Downsample(const std::vector<float>& data, int target_size); 

void Clear_Socket_Data();
//======================================

void View_Group_7(void) {    

    //=== Седьмая группа графиком данных из socket данных =================

    ImGui::BeginChild("Group 7", ImVec2(575, 270), true);
    ImGui::Text("Группа 7 - Данные из Socket");

    vector<int> new_parser_data;
    // Парсинг данных
    if (var.socket.init_socket_done) {

        if (!var.socket.hex_receive) {
            new_parser_data = parseSocketData(var.socket.data_prefix);

        } else {
            new_parser_data = parseBinarySocketData(); 

            if (var.log.log_Is_Started) {
                if (new_parser_data.size()) {                    
                    
                    Add_Str_To_Log_File_HEX(new_parser_data);
                }
            } 
        }
   
        parsed_data.insert(parsed_data.end(),
                new_parser_data.begin(),
                new_parser_data.end());

        var.debug.parser_data_size.store(parsed_data.size());
        var.debug.new_parser_data_size.store(new_parser_data.size());                   
    }

    //====  График значений ===========
    if (var.socket.chart_enable) {

        // Преобразование данных в формат для ImGui::PlotLines
        y_coords.clear();
        for (int number : parsed_data) {
            y_coords.push_back(static_cast<float>(number));
        }

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

        //===============================================================================


        // // В месте отрисовки:
        // int plot_width = static_cast<int>(available_size.x); // Количество пикселей по оси x
        // std::vector<float> plot_data = y_coords;

        // // Снижаем количество точек до ширины графика
        // if (y_coords.size() > static_cast<size_t>(plot_width)) {
        //     plot_data = Downsample(y_coords, plot_width);
        // }

        //=== скользящее окно ==========================================================
        // Размер скользящего окна
        // const size_t WINDOW_SIZE = 1000;
        // static float time = 0.0f;
        // static std::deque<float> data_window;

        // if (data_window.size() >= WINDOW_SIZE) {
        //     data_window.pop_front(); // Удаляем самый старый элемент
        // }
        // data_window.push_back(new_point);


        //=== скользящее окно ==========================================================

    
        // Отображение графика
        if (graph_type == 0) {
            // Рисуем график, используя доступную ширину и заданную высоту
            ImGui::PlotLines("Socket", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(available_size.x, plot_height));

        } else if (graph_type == 1) {
            ImGui::PlotHistogram("Socket", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
        } else if (graph_type == 2) {
            if (y_coords.size() > window_size) {
                ImGui::PlotLines("Socket", &y_coords[y_coords.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
            } else {
                ImGui::PlotLines("Socket", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
            }
        }
        // Кнопка для очистки графика
        if (ImGui::Button("Очистить график")) { 
            Clear_Socket_Data();
        }
        //================================

        ImGui::SameLine();
    }

    // Отображение количества пакетов в секунду с использованием EMA
    double alpha = 0.1; // Коэффициент сглаживания
   
    double current_packets_per_second = GetPacketsPerSecond_S();
    smoothed_packets_per_second.store(CalculateEMA_Socet(current_packets_per_second, smoothed_packets_per_second.load(), alpha));

    // Использование stringstream для форматирования строки
    stringstream ss;
    ss << fixed << setprecision(2) << "Пакетов в секунду: " << setw(6) << smoothed_packets_per_second.load();
    ImGui::Text("%s", ss.str().c_str());

    //============  Количество значений в секунду =======
    double current_val_data_per_second = Get_Val_Data_PerSecond_S(); //parsed_data.size();//
    smoothed_val_data_per_second.store(CalculateEMA_Socet(current_val_data_per_second, smoothed_val_data_per_second.load(), alpha));
    // Использование stringstream для форматирования строки
    stringstream ss1;
    ss1 << fixed << setprecision(2) << "Значений в секунду: " << setw(6) << smoothed_val_data_per_second.load();
    ImGui::Text("%s", ss1.str().c_str());

    //======================================================

    ImGui::EndChild();
}
//==================================================================================

// Функция для расчета экспоненциального скользящего среднего (EMA)
double CalculateEMA_Socet(double current_value, double previous_ema, double alpha) {
    return alpha * current_value + (1 - alpha) * previous_ema;
}
//==================================================================================


void Clear_Socket_Data() {
    lock_guard<mutex> lock(data_mutex);
    parsed_data.clear();    
    y_coords.clear();
}
//==================================================================================


#include <vector>
#include <algorithm>

// Функция для снижения количества точек
std::vector<float> Downsample(const std::vector<float>& data, int target_size) {
    std::vector<float> downsampled;
    if (data.empty() || target_size <= 0)
        return downsampled;

    int data_size = static_cast<int>(data.size());
    if (data_size <= target_size)
        return data; // Нет необходимости в снижении

    float ratio = static_cast<float>(data_size) / target_size;

    for (int i = 0; i < target_size; ++i) {
        int start = static_cast<int>(i * ratio);
        int end = static_cast<int>((i + 1) * ratio);
        end = std::min(end, data_size);

        float sum = 0.0f;
        for (int j = start; j < end; ++j)
            sum += data[j];
        float avg = sum / (end - start);
        downsampled.push_back(avg);
    }

    return downsampled;
}

