#include "main.h"
#include "init.h"
#include "defines.h"
#include "win_defines.h"

void View_Cv_Group_2() {

    ImGui::BeginChild("Group 2", ImVec2(CV2_GROUP_W, CV2_GROUP_H), true);
    ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 1;

    if (ImGui::RadioButton("min size", selected_radio == 0)) { 
        selected_radio = 0; 
        var.cv.frame_size = FRAME_SIZE_MIN;
        Close_CV();
        Init_CV();
    }
    if (ImGui::RadioButton("640x480 ", selected_radio == 1)) { 
        selected_radio = 1; 
        var.cv.frame_size = FRAME_SIZE_640;
        Close_CV();
        Init_CV();        
    }
    if (ImGui::RadioButton("max size", selected_radio == 2)) { 
        selected_radio = 2; 
        var.cv.frame_size = FRAME_SIZE_MAX;
        Close_CV();
        Init_CV();        
    }

    ImGui::EndChild();  
}
