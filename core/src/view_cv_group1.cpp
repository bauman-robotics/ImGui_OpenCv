#include "main.h"

int View_Cv_Group_1(cv::VideoCapture & cap) {

    ImGui::BeginChild("Group 1", ImVec2(300, 150), true);
   // ImGui::Text("Группа 1");

    // Добавляем кнопки управления

    if (ImGui::Button("                        Play                         ")) {
        var.cv.playing = true;
    }
    if (ImGui::Button("                        Pause                      ")) {
        var.cv.playing = false;
    } 
    if (ImGui::Button("                       Record                     ")) {
        if (!var.cv.recording) {
            var.cv.videoWriter.open("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, var.cv.frame.size());
            if (!var.cv.videoWriter.isOpened()) {
                std::cerr << "Error: Could not open video file for recording" << std::endl;
            } else {
                var.cv.recording = true;
            }
        }
    }
    if (ImGui::Button("                Stop Recording              ")) {
        if (var.cv.recording) {
            var.cv.videoWriter.release();
            var.cv.recording = false;
        }
    }

    ImGui::EndChild();

    return 0;

}