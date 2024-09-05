#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

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

int main(int, char**) {
    // Инициализируем камеру OpenCV
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    // Инициализируем GLFW
    if (!glfwInit()) {
        std::cerr << "Error: Could not initialize GLFW" << std::endl;
        return -1;
    }

    // Создаем окно и контекст OpenGL
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Camera Feed", NULL, NULL);
    if (!window) {
        std::cerr << "Error: Could not create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Делаем контекст окна текущим
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Включаем vsync

    // Инициализируем GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: Could not initialize GLEW" << std::endl;
        return -1;
    }

    // Инициализируем ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Настраиваем платформу/рендерер
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Создаем текстуру для отображения
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Логические флаги для управления воспроизведением
    bool playing = true;
    bool recording = false;
    cv::VideoWriter videoWriter;
    cv::Mat lastFrame; // Хранение последнего кадра для заморозки экрана

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        // Обрабатываем события
        glfwPollEvents();

        // Начинаем новый кадр ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
        showImage(texture, frame);

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
            break;
        }
        ImGui::End();

        // Записываем кадр, если запись включена
        if (recording && !frame.empty()) {
            videoWriter.write(frame);
        }

        // Рендерим
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Меняем буферы
        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    glDeleteTextures(1, &texture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}