#include "main.h"
#include "win_defines.h"
#include "view_groups.h"

void View_Group_Socket_Data_Ctrl() {

    ImGui::BeginChild("Socket_Signal_Ctrl", ImVec2(L1_P2_SOCKET_DATA_CTRL_W, L1_P2_SOCKET_DATA_CTRL_H), true);
    //ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 1;


    if (ImGui::RadioButton("Saw test", selected_radio == 0)) { 
        selected_radio = 0; 

        sprintf(var.socket.send.message, "%s", "Saw"); 
        var.socket.send.need_to_be_sended.store(1);
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Sin test", selected_radio == 1)) { 
        selected_radio = 1; 
      
        sprintf(var.socket.send.message, "%s", "Sin");         
        var.socket.send.need_to_be_sended.store(1); 
    }

    if (ImGui::RadioButton("Voltage", selected_radio == 2)) {
        selected_radio = 2; 
        sprintf(var.socket.send.message, "%s", "Voltage");          
        var.socket.send.need_to_be_sended.store(1);      
    }
    
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Current", selected_radio == 3)) {
        selected_radio = 3; 
        sprintf(var.socket.send.message, "%s", "Current");          
        var.socket.send.need_to_be_sended.store(1);      
    }

    ImGui::SameLine();
    
    if (ImGui::RadioButton("Pow.", selected_radio == 4)) {
        selected_radio = 4; 
        sprintf(var.socket.send.message, "%s", "Power");          
        var.socket.send.need_to_be_sended.store(1);      
    }


    static const char* combo_items[] = {"5", "10", "15", "20", "25", "50", "100", "200", "500", "1000", "2000", "3000", "5000" };
    //=========================================
    static int combo_current_item = 1;
    var.socket.packet_period_ms = 10;
    //=========================================
    // Устанавливаем ширину следующего элемента. Например, 200.0f пикселей.
    ImGui::SetNextItemWidth(70.0f);
    ImGui::Combo("Период пакетов, ms", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    if (ImGui::IsItemEdited())
    {
        //selected_radio = combo_current_item;
        //std::cout << "Период пакетов = " << combo_items[combo_current_item] << std::endl;

        var.socket.packet_period_ms = atoi(combo_items[combo_current_item]);
        sprintf(var.socket.send.message, "%s%s", "PER", combo_items[combo_current_item]);          
        var.socket.send.need_to_be_sended.store(1); 

    }

    //==========================================================================
    static const char* combo_items_num[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "20", "30", "40", "50", "100", "150", "200", "300", "350", "400", "500", "600", "700", "746", "800", "900", "1000"};
    //=========================================
    static int combo_current_item_num = 9;
    var.socket.val_in_packet = 10;
    //=========================================
    ImGui::SetNextItemWidth(70.0f);
    ImGui::Combo("Значений в пакете", &combo_current_item_num, combo_items_num, IM_ARRAYSIZE(combo_items_num));

    if (ImGui::IsItemEdited())
    {
        //selected_radio = combo_current_item;
        //std::cout << "Период пакетов = " << combo_items[combo_current_item] << std::endl;
        var.socket.val_in_packet = atoi(combo_items_num[combo_current_item_num]);
        sprintf(var.socket.send.message, "%s%s", "NUM", combo_items_num[combo_current_item_num]);          
        var.socket.send.need_to_be_sended.store(1); 
    }

    ImGui::EndChild();

}