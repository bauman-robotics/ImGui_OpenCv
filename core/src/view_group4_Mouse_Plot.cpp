#include "main.h"
#include "win_defines.h"

void View_Group_Mouse_Plot() {

    ImGuiIO& io = ImGui::GetIO();
    //ImGui::BeginChild("Group 4", ImVec2(865, 270), true);
    var.mouse.groupSize_x = MOUSE_PLOT_W;
    ImVec2 groupSize = ImVec2(var.mouse.groupSize_x, var.mouse.groupSize_y); // Начальный размер
    ImGui::BeginChild("Mouse_Plot", groupSize, true);


    ImGui::Text("Mouse_Plot");

    static std::vector<float> y_coords;
        // Проверка на валидность координат мыши
    bool mouse_valid = io.MousePos.x >= 0 && io.MousePos.y >= 0 && io.MousePos.x < io.DisplaySize.x && io.MousePos.y < io.DisplaySize.y;
    static int window_size = 100; // Размер окна последних значений
    static bool checkbox = var.mouse.mouse_chart_enable;

    if (var.mouse.mouse_chart_enable) {

        var.mouse.groupSize_y = MOUSE_PLOT_H_ON;

        // Добавление текущей Y координаты мыши в вектор только если координаты валидны
        if (mouse_valid) {
            y_coords.push_back(io.MousePos.y);
        }

        // Радиокнопки для выбора типа графика
        static int graph_type = 0; // 0 - линии, 1 - гистограмма, 2 - окно последних значений
        ImGui::RadioButton("Линии", &graph_type, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Гистограмма", &graph_type, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Окно последних значений", &graph_type, 2);

        // Слайдер для регулировки ширины окна последних значений
        
        if (graph_type == 2) {
            ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
        } else {
            ImGui::BeginDisabled();
            ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
            ImGui::EndDisabled();
        }

        // Отображение графика
        if (graph_type == 0) {
            ImGui::PlotLines("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        } else if (graph_type == 1) {
            ImGui::PlotHistogram("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        } else if (graph_type == 2) {
            if (y_coords.size() > window_size) {
                ImGui::PlotLines("Y координата мыши", &y_coords[y_coords.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
            } else {
                ImGui::PlotLines("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
            }
        }
        //========================================================
        
        ImGui::Checkbox("Mouse Chart Enable", &checkbox);

        if (ImGui::IsItemEdited())
        {
            if (checkbox) {
                var.mouse.mouse_chart_enable = 1;

                std::cout << "mouse_chart_enable" << std::endl;            
            } else {
                var.mouse.mouse_chart_enable = 0;
                std::cout << "mouse_chart_disable" << std::endl;
            }        
        }
        //========================================================
        ImGui::SameLine();

        // Кнопка для очистки графика
        if (ImGui::Button("Очистить график"))
        {
            y_coords.clear();
        }
    } else {
        var.mouse.groupSize_y = MOUSE_PLOT_H_OFF;
        
        ImGui::Checkbox("Mouse Chart Enable", &checkbox);

        if (ImGui::IsItemEdited())
        {
            if (checkbox) {
                var.mouse.mouse_chart_enable = 1;

                std::cout << "mouse_chart_enable" << std::endl;            
            } else {
                var.mouse.mouse_chart_enable = 0;
                std::cout << "mouse_chart_disable" << std::endl;
            }        
        }
    }


    ImGui::EndChild();

}