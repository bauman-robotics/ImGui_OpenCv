#include "main.h"
#include "uart.h"
#include "tcp-Server.h"
#include <atomic>


static std::atomic<double> smoothed_packets_per_second(0.0);
static std::atomic<double> smoothed_val_data_per_second(0.0);
static std::vector<float> y_coords;

static std::mutex data_mutex;
static std::vector<int> parsed_data;   

double CalculateEMA_Socet(double current_value, double previous_ema, double alpha);
void Clear_Socket_Data();
//======================================

void View_Group_7(void) {    

    //=== Седьмая группа графиком данных из socket данных =================

    ImGui::BeginChild("Group 7", ImVec2(575, 270), true);
    ImGui::Text("Группа 7 - Данные из Socket");

    std::vector<int> new_parser_data;
    // Парсинг данных
    if (var.socket.init_socket_done) {
        if (!var.socket.hex_receive) {
            new_parser_data = parseSocketData(var.socket.data_prefix);


        } else {
            new_parser_data = parseBinarySocketData(); 
        }

        parsed_data.insert(parsed_data.end(),
                new_parser_data.begin(),
                new_parser_data.end());


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

    
        // Отображение графика
        if (graph_type == 0) {
            // Рисуем график, используя доступную ширину и заданную высоту
            ImGui::PlotLines("Socket", y_coords.data(), static_cast<int>(y_coords.size()), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(available_size.x, plot_height));

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
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "Пакетов в секунду: " << std::setw(6) << smoothed_packets_per_second.load();
    ImGui::Text("%s", ss.str().c_str());

    //============  Количество значений в секунду =======
    double current_val_data_per_second = Get_Val_Data_PerSecond_S(); //parsed_data.size();//
    smoothed_val_data_per_second.store(CalculateEMA_Socet(current_val_data_per_second, smoothed_val_data_per_second.load(), alpha));
    // Использование stringstream для форматирования строки
    std::stringstream ss1;
    ss1 << std::fixed << std::setprecision(2) << "Значений в секунду: " << std::setw(6) << smoothed_val_data_per_second.load();
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
    std::lock_guard<std::mutex> lock(data_mutex);
    parsed_data.clear();    
    y_coords.clear();
}