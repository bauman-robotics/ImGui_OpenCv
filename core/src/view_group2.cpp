#include "main.h"

void View_Group_2() {

    ImGui::BeginChild("Group 2", ImVec2(300, 150), true);
    ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 0;
    static const char* combo_items[] = { "Строка 1", "Строка 2", "Строка 3" };
    static int combo_current_item = 0;

    if (ImGui::RadioButton("Опция 1", selected_radio == 0)) { selected_radio = 0; combo_current_item = 0; }
    if (ImGui::RadioButton("Опция 2", selected_radio == 1)) { selected_radio = 1; combo_current_item = 1; }
    if (ImGui::RadioButton("Опция 3", selected_radio == 2)) { selected_radio = 2; combo_current_item = 2; }

    ImGui::Combo("Комбобокс", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    // Обновление радиокнопок при изменении комбобокса
    if (ImGui::IsItemEdited())
    {
        selected_radio = combo_current_item;
    }

    ImGui::EndChild();

}