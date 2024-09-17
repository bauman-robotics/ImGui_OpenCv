#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "view_groups.h"
#include "variables.h"


void showImage();

//==============================================

// Функция для отображения изображения в ImGui
void showImage() {
    if (var.cv.frame.empty()) {
        std::cerr << "Error: Empty image" << std::endl;
        return;
    }

    // Конвертируем изображение из BGR в RGB
    cv::Mat imgRGB;
    cv::cvtColor(var.cv.frame, imgRGB, cv::COLOR_BGR2RGB);

    // Обновляем текстуру из изображения OpenCV
    glBindTexture(GL_TEXTURE_2D, var.cv.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgRGB.cols, imgRGB.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imgRGB.data);
    glBindTexture(GL_TEXTURE_2D, 0);

        // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(224, 38), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Camera Feed", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);

    // Отображаем текстуру в ImGui
    ImGui::Image((void*)(intptr_t)var.cv.texture, ImVec2(var.cv.frame.cols, var.cv.frame.rows));
    ImGui::End();
}
//======================================================================================

int Cv_Processing() {

    // Захватываем кадр
    if (var.cv.playing) {
        var.cv.cap >> var.cv.frame;
        if (!var.cv.frame.empty()) {
            var.cv.lastFrame = var.cv.frame.clone(); // Сохраняем последний кадр
        }
    } else {
        var.cv.frame = var.cv.lastFrame; // Используем последний сохраненный кадр
    }

    // Отображаем изображение в ImGui
    showImage();


    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(0, 38), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(224, 700), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoMove | 
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar);

    if (View_Cv_Group_1(var.cv.cap) == -1) {
        return -1;
    }

    View_Cv_Group_2();

    ImGui::End();

    // Записываем кадр, если запись включена
    if (var.cv.recording && !var.cv.frame.empty()) {
        var.cv.videoWriter.write(var.cv.frame);
    }
    return 0;
}