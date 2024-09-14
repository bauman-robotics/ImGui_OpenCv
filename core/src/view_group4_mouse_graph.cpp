#include "main.h"

void View_Group_4() {

    ImGuiIO& io = ImGui::GetIO();
    ImGui::BeginChild("Group 4", ImVec2(865, 270), true);
    ImGui::Text("Группа 4");

    static std::vector<float> y_coords;

    // Проверка на валидность координат мыши
    bool mouse_valid = io.MousePos.x >= 0 && io.MousePos.y >= 0 && io.MousePos.x < io.DisplaySize.x && io.MousePos.y < io.DisplaySize.y;

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
    static int window_size = 100; // Размер окна последних значений
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

    // Кнопка для очистки графика
    if (ImGui::Button("Очистить график"))
    {
        y_coords.clear();
    }

    ImGui::EndChild();

}