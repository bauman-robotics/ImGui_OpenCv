#include "main.h"

void View_Group_2() {

    ImGui::BeginChild("Group 2", ImVec2(300, 150), true);
    ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 1;
    static const char* combo_items[] = { "Red", "Green", "Blue" };
    static int combo_current_item = 1;

    if (ImGui::RadioButton("Red", selected_radio == 0)) { 
        selected_radio = 0; 
        combo_current_item = 0; 
        //var.socket.send.str = "Red";
        sprintf(var.socket.send.message, "%s", "Red"); 
        var.socket.send.need_to_be_sended.store(1);
    }
    if (ImGui::RadioButton("Green", selected_radio == 1)) { 
        selected_radio = 1; 
        combo_current_item = 1; 
        //var.socket.send.str = "Green";
        sprintf(var.socket.send.message, "%s", "Green");         
        var.socket.send.need_to_be_sended.store(1);       
    }
    if (ImGui::RadioButton("Blue", selected_radio == 2)) {
        selected_radio = 2; 
        combo_current_item = 2; 
        //var.socket.send.str = "Blue";
        sprintf(var.socket.send.message, "%s", "Blue");          
        var.socket.send.need_to_be_sended.store(1);      
    }

    ImGui::Combo("Светодиод", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    // Обновление радиокнопок при изменении комбобокса
    if (ImGui::IsItemEdited())
    {
        selected_radio = combo_current_item;
    }

    ImGui::EndChild();

}