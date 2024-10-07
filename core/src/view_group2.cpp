#include "main.h"

void View_Group_2() {

    ImGui::BeginChild("Group 2", ImVec2(300, 150), true);
    //ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 1;


    if (ImGui::RadioButton("Red - Saw mode", selected_radio == 0)) { 
        selected_radio = 0; 

        sprintf(var.socket.send.message, "%s", "Red"); 
        var.socket.send.need_to_be_sended.store(1);
    }
    if (ImGui::RadioButton("Green - Sin mode", selected_radio == 1)) { 
        selected_radio = 1; 
      
        sprintf(var.socket.send.message, "%s", "Green");         
        var.socket.send.need_to_be_sended.store(1);       
    }
    if (ImGui::RadioButton("Blue - Triangle mode", selected_radio == 2)) {
        selected_radio = 2; 
        sprintf(var.socket.send.message, "%s", "Blue");          
        var.socket.send.need_to_be_sended.store(1);      
    }

    static const char* combo_items[] = {"5", "10", "15", "20", "25", "50", "100", "200", "500", "1000", "2000", "3000", "5000" };
    static int combo_current_item = 1;

    // Устанавливаем ширину следующего элемента. Например, 200.0f пикселей.
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("Период пакетов, ms", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    if (ImGui::IsItemEdited())
    {
        //selected_radio = combo_current_item;
        //std::cout << "Период пакетов = " << combo_items[combo_current_item] << std::endl;

        sprintf(var.socket.send.message, "%s%s", "PER", combo_items[combo_current_item]);          
        var.socket.send.need_to_be_sended.store(1); 
    }

    //==========================================================================
    static const char* combo_items_num[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30", "40", "50", "100", "150", "200", "300", "350", "400", "500", "600", "700", "746", "800", "900", "1000"};
    static int combo_current_item_num = 9;
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("Значений в пакете", &combo_current_item_num, combo_items_num, IM_ARRAYSIZE(combo_items_num));

    if (ImGui::IsItemEdited())
    {
        //selected_radio = combo_current_item;
        //std::cout << "Период пакетов = " << combo_items[combo_current_item] << std::endl;

        sprintf(var.socket.send.message, "%s%s", "NUM", combo_items_num[combo_current_item_num]);          
        var.socket.send.need_to_be_sended.store(1); 
    }

    ImGui::EndChild();

}