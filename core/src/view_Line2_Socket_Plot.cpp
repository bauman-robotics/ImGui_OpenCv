#include "main.h"
#include "uart.h"
#include "tcp-Server.h"
#include <atomic>

#include <iostream>
#include <vector>
#include <string>
#include "defines.h"
#include "win_defines.h"

#include <deque>
#include <algorithm>

#include <chrono>
#include <ctime>


static vector<float> y_coords;

static mutex data_mutex;
static vector<int> parsed_data;   
static bool freeze_en = 0;

extern atomic<double> smoothed_val_data_per_second_s;

using namespace std;

void Pars_Data_And_Binary_Log(); 

void Clear_Socket_Data();
void plotData(float y_min, float y_max, ImVec2 available_size, std::vector<float>& data); 
std::vector<float> decimateVector(const std::vector<float>& input, size_t decimationThreshold); 
std::vector<float> movingAverage(const std::vector<float>& input, size_t windowSize);
std::vector<float> medianFilter(const std::vector<float>& input, size_t windowSize);
//==================================================================================

void View_Group_Socket_Plot(void) {    

    //=== Седьмая группа графиком данных из socket данных =================

    //ImGui::BeginChild("Socket_Plot", ImVec2(575, 270), true);
    ImGui::BeginChild("Socket_Plot", ImVec2(L2_P1_SOCKET_PLOT_W, L2_P1_SOCKET_PLOT_H), true);    
    //ImGui::Text("Данные из Socket");

    if (!freeze_en) {
        Pars_Data_And_Binary_Log();
    }

    //====  График значений ===========
    if (var.socket.chart_enable) {

        // Радиокнопки для выбора типа графика
        static int old_graph_type = 0; 
        static int graph_type = 0; // 0 - линии, 1 - гистограмма, 2 - окно последних значений
        ImGui::RadioButton("Линии", &graph_type, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Гистограмма", &graph_type, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Окно", &graph_type, 2);

        ImGui::SameLine();
        //==================================================================================
        // Слайдер для регулировки ширины окна последних значений
        static int window_size = 1000; // Размер окна последних значений
        if (graph_type == 2) {
            ImGui::SliderInt(" ", &window_size, 1, 10000);
        } else {
            ImGui::BeginDisabled();
            ImGui::SliderInt(" ", &window_size, 1, 10000);
            ImGui::EndDisabled();
        }
        if ((graph_type != old_graph_type) && (graph_type == 2)) {
            Clear_Socket_Data();
            cout << "Clear_Socket_Data  "  << endl;
            old_graph_type = graph_type;
        }


        // Получаем доступную ширину контента
        ImVec2 available_size = ImGui::GetContentRegionAvail();

        // Устанавливаем желаемую высоту графика
        float plot_height = L2_PLOT_HEIGHT;
        //===============================================================================


        //===================================================================
        // Получаем позицию курсора, которая будет нижней левой точкой области графика после его отрисовки
        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
        // Перемещаем курсор после графика, чтобы следующие элементы рисовались ниже
        ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y + 10));

        //===  Сетка графика =====================================================

        float y_min = 0;
        float y_max = 10;
        int label_precision = 2;       
        
        // Проверяем, что вектор с данными не пустой
        if (!var.socket.data_f.empty()) {
            // Находим минимальное и максимальное значения в векторе
            if (graph_type == 2) {
                if (var.socket.data_f.size() > window_size) {
                    y_min = *std::min_element(var.socket.data_f.end() - window_size, var.socket.data_f.end());
                    y_max = *std::max_element(var.socket.data_f.end() - window_size, var.socket.data_f.end());
                } else {
                    y_min = *std::min_element(var.socket.data_f.begin(), var.socket.data_f.end());
                    y_max = *std::max_element(var.socket.data_f.begin(), var.socket.data_f.end());
                }
            } else {
                y_min = *std::min_element(var.socket.data_f.begin(), var.socket.data_f.end());
                y_max = *std::max_element(var.socket.data_f.begin(), var.socket.data_f.end());
            }            
        }
        // === Количество знаков после запятой, метки оси Y ===
        if (((y_max - y_min) < 0.001))                              label_precision = 5;
        if (((y_max - y_min) >=0.001) && ((y_max - y_min) < 0.01))  label_precision = 4;
        if (((y_max - y_min) >= 0.01) && ((y_max - y_min) <  0.1))  label_precision = 3;
        if (((y_max - y_min) >=  0.1) && ((y_max - y_min) <    1))  label_precision = 2;
        if (((y_max - y_min) >=    1) && ((y_max - y_min) <   10))  label_precision = 1;
        if ((y_max - y_min)  >=10)                                  label_precision = 0;
        //====================================================

        // Рассчитываем шаг сетки по оси Y
        const int grid_lines_y = 10;
        const float y_step = plot_height / grid_lines_y;

        // Рассчитываем шаг сетки по оси X
        const int grid_lines_x = 10;
        const float x_step = available_size.x / grid_lines_x; 

        // Получаем начальную позицию для рисования графика
        ImVec2 graph_start = ImGui::GetCursorScreenPos();

        // Рисуем вертикальную сетку и метки оси Y
        for (int i = 0; i <= grid_lines_y; ++i) {
            float y_pos = graph_start.y + plot_height - (y_step * i);
            ImVec2 p1 = ImVec2(graph_start.x, y_pos);
            ImVec2 p2 = ImVec2(graph_start.x + available_size.x, y_pos);
            ImGui::GetWindowDrawList()->AddLine(p1, p2, IM_COL32(200, 200, 200, 100)); 

            // Форматируем текст метки с ограничением количества знаков после запятой
            std::ostringstream label_stream;
            label_stream << std::fixed << std::setprecision(label_precision) << (y_min + (y_max - y_min) * i / grid_lines_y);
            std::string label = label_stream.str();

            // Рассчитываем позицию текста
            ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
            ImVec2 text_pos = ImVec2(graph_start.x + 5, y_pos - text_size.y * 0.5f);            
            //ImGui::GetWindowDrawList()->AddText(text_pos, IM_COL32(255, 255, 255, 255), label.c_str()); 
            ImGui::GetWindowDrawList()->AddText(text_pos, IM_COL32(255, 255, 0, 255), label.c_str()); 
        }        

        //int values_per_second = (int)smoothed_val_data_per_second_s.load();
        int values_per_second = 0;
        if (var.socket.packet_period_ms != 0) {
            values_per_second = (int)((float)var.socket.val_in_packet * 1000 / var.socket.packet_period_ms);
            
        }

        // Рисуем горизонтальную сетку и временные метки
        for (int i = 0; i <= grid_lines_x; ++i) {
            float x_pos = graph_start.x + (x_step * i);
            ImVec2 p1 = ImVec2(x_pos, graph_start.y);
            ImVec2 p2 = ImVec2(x_pos, graph_start.y + plot_height);
            ImGui::GetWindowDrawList()->AddLine(p1, p2, IM_COL32(200, 200, 200, 100)); 

            // === Рисование временных меток ===
            if (values_per_second > 0) {
                float time_value = 0;
                // Рассчитываем время в секундах для каждой линии сетки
                if ((graph_type == 2) && (var.socket.data_f.size() > window_size)) {
                    time_value = (float)i / grid_lines_x * window_size / values_per_second;
                } else {    
                    time_value = (float)i / grid_lines_x * var.socket.data_f.size() / values_per_second;
                }

                // Форматируем время в виде "секунды.миллисекунды"
                std::ostringstream time_label_stream;
                time_label_stream << std::fixed << std::setprecision(2) << time_value;
                std::string time_label = time_label_stream.str();

                // Рассчитываем позицию текста временной метки
                ImVec2 time_label_size = ImGui::CalcTextSize(time_label.c_str());
                float p_label_x = x_pos - time_label_size.x / 2;
                if (i == 0) p_label_x = graph_start.x; 
                if (i == grid_lines_x) p_label_x = p_label_x - time_label_size.x / 2;               
                ImVec2 time_label_pos = ImVec2(p_label_x, graph_start.y + plot_height + 5); // 5 - отступ от графика
                ImGui::GetWindowDrawList()->AddText(time_label_pos, IM_COL32(255, 255, 255, 255), time_label.c_str());
            }
            // ===================================
        }

        //==============================================================

        // Отображение графика
        if (graph_type == 0) {
            // Рисуем график, используя доступную ширину и заданную высоту
            ImGui::PlotLines("Socket", var.socket.data_f.data(), var.socket.data_f.size(), 0, NULL, y_min, y_max, ImVec2(available_size.x, plot_height));
            //plotData(y_min, y_max, ImVec2(available_size.x, plot_height), var.socket.data_f);
        } else if (graph_type == 1) {
            ImGui::PlotHistogram("Socket", var.socket.data_f.data(), var.socket.data_f.size(), 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
        } else if (graph_type == 2) {
            if (var.socket.data_f.size() > window_size) {
                ImGui::PlotLines("Socket", &var.socket.data_f[var.socket.data_f.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
            } else {
                ImGui::PlotLines("Socket", var.socket.data_f.data(), var.socket.data_f.size(), 0, NULL, FLT_MAX, FLT_MAX,  ImVec2(available_size.x, plot_height));
            }
        }

        //===================================================================
        // Получаем позицию курсора, которая будет нижней левой точкой области графика после его отрисовки
        cursor_pos = ImGui::GetCursorScreenPos();
        // Перемещаем курсор после графика, чтобы следующие элементы рисовались ниже
        ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y + 30));
   
        //============================================================

        // Кнопка для очистки графика
        if (ImGui::Button("Очистить график")) { 
            Clear_Socket_Data();
        }
        //================================

        ImGui::SameLine();
        //========================================

        ImGui::Checkbox("Freeze", &freeze_en);

    }

    //======================================================

    ImGui::EndChild();
}
//==================================================================================

void Clear_Socket_Data() {
    lock_guard<mutex> lock(data_mutex);
    //parsed_data.clear();
    var.socket.data_f.clear();
    y_coords.clear();
}
//==================================================================================

void Pars_Data_And_Binary_Log() {

    // Парсинг данных
    if (var.socket.init_socket_done) {

        if (!var.socket.hex_receive) {

           y_coords = parseSocketData_Float(var.socket.data_prefix);
           #ifdef LOG_ONE_VAL_TO_LINE
                if (var.log.log_Is_Started) {
                    if (y_coords.size()) {                    
                   
                        Add_Str_To_Log_File_HEX_Float(y_coords);
                     }
                } 
           #endif 

        } else {
            // y_coords - only for logs 
            //==== output ====
            // var.socket.data_f 
            y_coords =  parseBinarySocketData_Float();  

            if (var.log.log_Is_Started) {
                if (y_coords.size()) {                    
                    
                    Add_Str_To_Log_File_HEX_Float(y_coords);
                }
            } 
        }               
    }
}  
//==================================================================================

// Функция для децимации вектора в два раза
std::vector<float> decimateVector(const std::vector<float>& input, size_t decimationThreshold) {
    std::vector<float> output;
    if (input.size() > decimationThreshold) {
        for (size_t i = 0; i < input.size(); i += 2) {
            output.push_back((input[i] + input[i + 1]) / 2.0f);
        }
    } else {
        output = input;
    }
    return output;
}

// std::vector<float> decimateVector(const std::vector<float>& input, size_t decimationThreshold) {
//   std::vector<float> output;
//   if (input.size() > decimationThreshold) {
//     for (size_t i = 0; i < input.size(); i += 2) {
//       output.push_back(std::max(input[i], input[i + 1]));
//     }
//   } else {
//     output = input;
//   }
//   return output;
// }


// std::vector<float> decimateVector(const std::vector<float>& input, size_t decimationThreshold) {
//     std::vector<float> output;
//     if (input.size() > decimationThreshold) {
//         if (input.size() >= 4) {
//             for (size_t i = 0; i < input.size(); i += 4) {
//             float sum = input[i] + input[i + 1] + input[i + 2] + input[i + 3];
//             output.push_back(sum / 4.0f);
//             }
//         }
//     }  else {
//         output = input;
//     }
//   return output;
// }
//==================================================================================


void plotData(float y_min, float y_max, ImVec2 available_size, std::vector<float>& data) {
    // Проверяем, нужно ли децимировать данные
    if (data.size() > 1 * available_size.x) {
        data = decimateVector(data, 2 * available_size.x);        
    }

    //size_t windowSize = static_cast<size_t>(2 * available_size.x / data.size());
    //data = movingAverage(data, windowSize);
    //data = medianFilter(data, windowSize);

    ImGui::PlotLines("Socket", &data[0], data.size(), 0, NULL, y_min, y_max, ImVec2(available_size.x, available_size.y));
}
//==================================================================================


// Функция для вычисления среднего скользящего
std::vector<float> movingAverage(const std::vector<float>& input, size_t windowSize) {
    std::vector<float> output;
    for (size_t i = 0; i < input.size(); ++i) {
        if (i + windowSize <= input.size()) {
            float sum = 0.0f;
            for (size_t j = 0; j < windowSize; ++j) {
                sum += input[i + j];
            }
            output.push_back(sum / windowSize);
        } else {
            break; // Остановка, если окно выходит за границы вектора
        }
    }
    return output;
}
//==================================================================================

// Функция для медианного фильтрования
std::vector<float> medianFilter(const std::vector<float>& input, size_t windowSize) {
    std::vector<float> output;
    for (size_t i = 0; i < input.size(); ++i) {
        if (i + windowSize <= input.size()) {
            std::vector<float> window(input.begin() + i, input.begin() + i + windowSize);
            std::sort(window.begin(), window.end());
            output.push_back(window[windowSize / 2]);
        } else {
            break; // Остановка, если окно выходит за границы вектора
        }
    }
    return output;
}
