#include "main.h"
#include "view.h"
#include "uart.h"
#include "view_groups.h"

void ShowDemoWindow(int serial_fd)
{
    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(880, 720), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Demo Window", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    //=== Первая группа с рамкой ========================================================================
    View_Group_1();

    // Разделитель между группами
    ImGui::SameLine();

    //=== Вторая группа с рамкой ========================================================================

    View_Group_2();
    // Разделитель между группами
    ImGui::SameLine();

    //=== Третья группа с рамкой ========================================================================

    View_Group_3();

    //=== Четвертая группа с графиком координат мыши ====================================================

    View_Group_4();

    //=== Пятая группа с листбоксом данных из последовательного порта =================
    #ifdef USE_COM_PORT

        View_Group_5(serial_fd);

    #endif 

    // Конец окна
    ImGui::End();
}