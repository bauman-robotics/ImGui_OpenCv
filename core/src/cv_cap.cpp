#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>


void showImage(GLuint texture, const cv::Mat& img);

//==============================================

// Функция для отображения изображения в ImGui
void showImage(GLuint texture, const cv::Mat& img) {
    if (img.empty()) {
        std::cerr << "Error: Empty image" << std::endl;
        return;
    }

    // Конвертируем изображение из BGR в RGB
    cv::Mat imgRGB;
    cv::cvtColor(img, imgRGB, cv::COLOR_BGR2RGB);

    // Обновляем текстуру из изображения OpenCV
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgRGB.cols, imgRGB.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imgRGB.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Отображаем текстуру в ImGui
    ImGui::Begin("Camera Feed");
    ImGui::Image((void*)(intptr_t)texture, ImVec2(img.cols, img.rows));
    ImGui::End();
}
//======================================================================================

int Cv_Processing(cv::VideoCapture & cap, GLuint * texture) {
    // Логические флаги для управления воспроизведением
    static bool playing = true;
    static bool recording = false;
    static cv::Mat lastFrame; // Хранение последнего кадра для заморозки экрана
    static cv::VideoWriter videoWriter;

    // Захватываем кадр
    cv::Mat frame;
    if (playing) {
        cap >> frame;
        if (!frame.empty()) {
            lastFrame = frame.clone(); // Сохраняем последний кадр
        }
    } else {
        frame = lastFrame; // Используем последний сохраненный кадр
    }

    // Отображаем изображение в ImGui
    showImage(*texture, frame);

    // Добавляем кнопки управления
    ImGui::Begin("Controls");
    if (ImGui::Button("Play")) {
        playing = true;
    }
    if (ImGui::Button("Pause")) {
        playing = false;
    }
    if (ImGui::Button("Record")) {
        if (!recording) {
            videoWriter.open("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, frame.size());
            if (!videoWriter.isOpened()) {
                std::cerr << "Error: Could not open video file for recording" << std::endl;
            } else {
                recording = true;
            }
        }
    }
    if (ImGui::Button("Stop Recording")) {
        if (recording) {
            videoWriter.release();
            recording = false;
        }
    }
    if (ImGui::Button("Close Stream")) {
        cap.release();
        return -1;
    }
    ImGui::End();

    // Записываем кадр, если запись включена
    if (recording && !frame.empty()) {
        videoWriter.write(frame);
    }
    return 0;
}