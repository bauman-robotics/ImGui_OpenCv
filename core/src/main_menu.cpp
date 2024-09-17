    #include "main.h"
    
void Menu_Item(void) {
    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(880, 38), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Menu Window", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);

    // Цвета для подсвечивания выбранной кнопки
    ImVec4 activeColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // Цвет для активной кнопки
    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Button); // Цвет по умолчанию

    // Добавляем кнопки управления
    ImGui::PushStyleColor(ImGuiCol_Button, var.com_port_mode ? activeColor : defaultColor);
    if (ImGui::Button("                         Com-port                           ")) {

        Select_Mode(COM_PORT_MODE);       
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, var.ctrl_mode ? activeColor : defaultColor);
    if (ImGui::Button("                         Ctrl                             ")) {
        
        Select_Mode(CTRL_MODE);  
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, var.cv_mode ? activeColor : defaultColor);
    if (ImGui::Button("                         OpenCv                           ")) {

        Select_Mode(OPENCV_MODE); 

    }
    ImGui::PopStyleColor();

    ImGui::End();
}