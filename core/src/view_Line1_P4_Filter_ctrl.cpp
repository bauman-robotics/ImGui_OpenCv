#include "main.h"
#include "uart.h"
// #include <atomic>
#include <vector>
#include <string>
#include <filesystem>

#include <tcp-Server.h>
#include <view_groups.h>

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include "log_file.h"
#include "defines.h"
#include "win_defines.h"

using namespace std;
namespace fs = filesystem;


//======================================

void View_Group_Line1_P4_Filter_Ctrl() {    

    ImGui::BeginChild("Socket_Filter", ImVec2(L1_P4_SOCKET_FILTER_CTRL_W, L1_P4_SOCKET_FILTER_CTRL_H), true);

    static const char* combo_items_num[] =  {"1", "3", "5", "10", "20", "50", "100", "200", "300", "400", "500", "1000" };
    //=========================================
    static int combo_current_item_num = 6; // Порядок фильтра 100

    //=========================================
    ImGui::SetNextItemWidth(60.0f);
    ImGui::Combo("Filter V", &combo_current_item_num, combo_items_num, IM_ARRAYSIZE(combo_items_num));

    if (ImGui::IsItemEdited())
    {
        var.socket.ina226.filter.order_V = atoi(combo_items_num[combo_current_item_num]);
        sprintf(var.socket.send.message, "%s%s", "V_FILTER_ORDER", combo_items_num[combo_current_item_num]);          
        var.socket.send.need_to_be_sended.store(1); 
    }

   //==========================================================================

    static const char* combo_items[] = {"1", "3", "5", "10", "20", "50", "100", "200", "300", "400", "500", "1000" };
    //=========================================
    static int combo_current_item = 6; // Порядок фильтра 100
    //=========================================
    // Устанавливаем ширину следующего элемента. Например, 200.0f пикселей.
    ImGui::SetNextItemWidth(60.0f);
    ImGui::Combo("Filter I", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    if (ImGui::IsItemEdited())
    {
        var.socket.ina226.filter.order_I = atoi(combo_items[combo_current_item]);
        sprintf(var.socket.send.message, "%s%s", "I_FILTER_ORDER", combo_items[combo_current_item]);          
        var.socket.send.need_to_be_sended.store(1); 
    }


   //==========================================================================

    static const char* combo_items_i_lim[] = {"52", "75", "103", "200", "300", "400", "500", "1000", "1638", "2000", "3000", "3160" };
    //=========================================
    static int combo_i_lim_item = 7; // 1A max current
    //=========================================
    // Устанавливаем ширину следующего элемента. Например, 200.0f пикселей.
    ImGui::SetNextItemWidth(60.0f);
    ImGui::Combo("I Lim, mA", &combo_i_lim_item, combo_items_i_lim, IM_ARRAYSIZE(combo_items_i_lim));

    var.socket.ina226.i_lim_mA = atoi(combo_items_i_lim[combo_i_lim_item]);

    if (ImGui::IsItemEdited())
    {
        sprintf(var.socket.send.message, "%s%s", "I_LIM_SET", combo_items_i_lim[combo_i_lim_item]);          
        var.socket.send.need_to_be_sended.store(1); 
    }
    ImGui::Text("LSB i = %0.2f mkA", (float)var.socket.ina226.i_lim_mA * 1000 / 32768);
    ImGui::Text("LSB V = 1.25 mV");
    ImGui::EndChild();
}
//==================================================================================
