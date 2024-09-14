#include "main.h"
#include "uart.h"

void View_Group_5(void) {    

    //=== Пятая группа графиком даннх из последовательного порта работающего в отдельном потоке =================

    std::vector<float> y_coords;

    ImGui::BeginChild("Group 5", ImVec2(865, 270), true);
    ImGui::Text("Группа 5 - Данные из последовательного порта");

    // Парсинг данных
    std::vector<int> parsed_data = parseComPortData();

    //====  График значений ===========

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

    // Отображение графика
    if (graph_type == 0) {
        ImGui::PlotLines("Данные из ком-порта", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 1) {
        ImGui::PlotHistogram("Данные из ком-порта", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 2) {
        if (y_coords.size() > window_size) {
            ImGui::PlotLines("Данные из ком-порта", &y_coords[y_coords.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        } else {
            ImGui::PlotLines("Данные из ком-порта", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        }
    }
    //==================================================================================
    //ImGui::PlotLines("Данные из ком-порта", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    //================================

    // Кнопка для очистки графика
    if (ImGui::Button("Очистить график")) {
        y_coords.clear();
        ClearSerialData();         
    }
    //================================

    ImGui::EndChild();

}