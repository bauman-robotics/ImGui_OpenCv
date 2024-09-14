#include "main.h"

void View_Group_3() {

    ImGui::BeginChild("Group 3", ImVec2(250, 150), true);
    ImGui::Text("Группа 3");

    ImGuiIO& io = ImGui::GetIO();
    static float wheel_rotation = 0.0f;
    static int wheel_click_count = 0;

    // Обновление количества нажатий на колесико
    if (io.MouseClicked[2]) {
        wheel_click_count++;
    }

    // Обновление поворота колесика
    wheel_rotation += io.MouseWheel;

    // Проверка на валидность координат мыши
    bool mouse_valid = io.MousePos.x >= 0 && io.MousePos.y >= 0 && io.MousePos.x < io.DisplaySize.x && io.MousePos.y < io.DisplaySize.y;

    if (mouse_valid) {
        ImGui::Text("Координаты курсора: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
    } else {
        ImGui::Text("Координаты курсора: (не в окне)");
    }
    ImGui::Text("Левая кнопка: %s", io.MouseDown[0] ? "Нажата" : "Не нажата");
    ImGui::Text("Поворот колесика: %.1f", wheel_rotation);
    ImGui::Text("Нажатия колесика: %d", wheel_click_count);
    ImGui::Text("Правая кнопка: %s", io.MouseDown[1] ? "Нажата" : "Не нажата");

    ImGui::EndChild();

}