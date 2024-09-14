#include "main.h"
#include "uart.h"

void View_Group_5(int serial_fd) {

    //=== Пятая группа с листбоксом данных из последовательного порта =================

    std::vector<float> y_coords;

    ImGui::BeginChild("Group 5", ImVec2(865, 200), true);
    ImGui::Text("Группа 5 - Данные из последовательного порта");

    static std::vector<std::string> serial_data;

    // Чтение данных из последовательного порта
    ReadSerialData(serial_fd, serial_data);

    // Парсинг данных
    std::vector<int> parsed_data = parseComPortData(serial_data);

    //====  Парсинг значения для Listbox ===========

    // Преобразование данных в формат для ImGui::ListBox
    // std::vector<std::string> serial_data_strings;
    // for (int number : parsed_data) {
    //     serial_data_strings.push_back(std::to_string(number));
    // }

    // std::vector<const char*> serial_data_cstr;
    // for (const auto& str : serial_data_strings) {
    //     serial_data_cstr.push_back(str.c_str());
    // }

    // static int item_current2 = 0;
    // ImGui::ListBox("Листбокс 2", &item_current2, serial_data_cstr.data(), serial_data_cstr.size());
    //=================================


    //====  График значения ===========

    // Преобразование данных в формат для ImGui::PlotLines
    y_coords.clear();
    for (int number : parsed_data) {
        y_coords.push_back(static_cast<float>(number));
    }

    ImGui::PlotLines("Данные из ком-порта", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    //================================

    //====  Сырые данные ===========

    // Отображение данных в листбоксе
    // static int item_current2 = 0;
    // std::vector<const char*> serial_data_cstr;
    // for (const auto& item : serial_data)
    // {
    //     serial_data_cstr.push_back(item.c_str());
    // }

    // ImGui::ListBox("Листбокс 2", &item_current2, serial_data_cstr.data(), serial_data_cstr.size());
    //================================

    ImGui::EndChild();

}