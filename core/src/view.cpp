#include "main.h"
#include "view.h"
#include "uart.h"
#include "view_groups.h"
#include "defines.h"
#include "win_defines.h"

void ShowCtrlWindow() {

    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(MAIN_WIN_X_POS, MAIN_WIN_Y_POS), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(MAIN_WIN_WIDTH, MAIN_WIN_HIGH), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Ctrl Window", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);
    #ifndef DISABLE_MOUSE_TEAM
    //=== Первая группа с рамкой ========================================================================
    View_Group_Test_Ctrls();

    // Разделитель между группами
    ImGui::SameLine();
    #endif 
   

    if (var.ctrl_mode) {        
        View_Group_Socket_Port_Ctrl();
        ImGui::SameLine();
        View_Group_Socket_Data_Ctrl();
        ImGui::SameLine();
        View_Group_9_Socket_Logs();
        ImGui::SameLine();
        View_Group_Line1_P4_Filter_Ctrl();
        View_Group_Socket_Com_Plot();
    }    


    #ifndef DISABLE_MOUSE_TEAM
        if (var.mouse.mouse_chart_enable) {
            // Разделитель между группами
            ImGui::SameLine();

            //=== Третья группа с рамкой ========================================================================

            View_Group_Mouse_Ctrl();
        }

        //=== Четвертая группа с графиком координат мыши ====================================================

        View_Group_Mouse_Plot();
    #else

    #endif 

    // //=== Пятая и шестая группы с листбоксом данных из последовательного порта =================
    // if (var.com_port_mode) {
    //     View_Group_Serial_Plot();
    //     ImGui::SameLine();
    //     View_Group_Serial_Ctrl();
    // }


    
    if (var.com_port_mode) {
        View_Group_Serial_Ctrl();
        ImGui::SameLine();
        View_Group_Socket_Data_Ctrl();
        ImGui::SameLine();
        View_Group_9_Socket_Logs();
        ImGui::SameLine();
        View_Group_Line1_P4_Filter_Ctrl();
        View_Group_Socket_Com_Plot();

        //View_Group_Serial_Plot();

    }


    //=== Седьмая и восьмая группы с листбоксом данных из socket порта =================
    // if (var.ctrl_mode) {
    //     //View_Group_Socket_Com_Plot();
    //     //ImGui::SameLine();
    //     //View_Group_Socket_Port_Ctrl();
    // }    
    // Конец окна
    ImGui::End();
}