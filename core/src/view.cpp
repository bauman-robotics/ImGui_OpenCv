#include "main.h"
#include "view.h"
#include "uart.h"
#include "view_groups.h"


void ShowCtrlWindow() {

    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(0, 38), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(880, 736), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Ctrl Window", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);

    //=== Первая группа с рамкой ========================================================================
    View_Group_1();

    // Разделитель между группами
    ImGui::SameLine();

    //=== Вторая группа с рамкой ========================================================================

    View_Group_2();

    if (var.mouse.mouse_chart_enable) {
        // Разделитель между группами
        ImGui::SameLine();

        //=== Третья группа с рамкой ========================================================================

        View_Group_3();

    }

    //=== Четвертая группа с графиком координат мыши ====================================================

    View_Group_4();

    //=== Пятая и шестая группы с листбоксом данных из последовательного порта =================
    if (var.com_port_mode) {
        View_Group_5();
        ImGui::SameLine();
        View_Group_6();
    }

    //=== Седьмая и восьмая группы с листбоксом данных из socket порта =================
    if (var.ctrl_mode) {
        View_Group_7();
        ImGui::SameLine();
        View_Group_8();
    }    
    // Конец окна
    ImGui::End();
}