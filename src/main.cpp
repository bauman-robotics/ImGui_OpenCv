//#define USE_OPENCV

#ifdef USE_OPENCV

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

	// //Выключение возможности изменения размера окна
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


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

#else

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

const char font_path[] = "/home/andrey/projects/ImGui_OpenCv/src/Roboto-Italic.ttf";

void LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault(); // Загрузка стандартного шрифта

    // Загрузка шрифта с поддержкой кириллицы
    ImFont* font = io.Fonts->AddFontFromFileTTF(font_path, 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    if (font == NULL)
    {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
    }
    else
    {
        io.FontDefault = font; // Установка загруженного шрифта как шрифта по умолчанию
    }
}

void ShowDemoWindow()
{
    // Установка позиции и размера окна
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(900, 900), ImGuiCond_Always);

    // Начало нового окна
    ImGui::Begin("Demo Window", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    static std::vector<std::string> items1 = { "Item 1", "Item 2", "Item 3" };
    static std::vector<const char*> items1_cstr;
    static int item_current1 = 0;

    // Первая группа с рамкой
    ImGui::BeginChild("Group 1", ImVec2(200, 200), true);
    ImGui::Text("Группа 1");

    // Кнопка для добавления элемента
    if (ImGui::Button("Добавить элемент"))
    {
        // Добавление нового элемента в листбокс
        items1.push_back("Новый элемент " + std::to_string(items1.size() + 1));
        items1_cstr.clear();
        for (const auto& item : items1)
        {
            items1_cstr.push_back(item.c_str());
        }
    }

    // Кнопка для очистки списка
    ImGui::SameLine();
    if (ImGui::Button("Очистить список"))
    {
        // Очистка списка
        items1.clear();
        items1_cstr.clear();
    }

    static bool checkbox1 = false;
    ImGui::Checkbox("Отметить меня 1", &checkbox1);
    ImGui::ListBox("Листбокс 1", &item_current1, items1_cstr.data(), items1_cstr.size());
    ImGui::EndChild();

    // Разделитель между группами
    ImGui::SameLine();

    // Вторая группа с рамкой
    ImGui::BeginChild("Group 2", ImVec2(200, 200), true);
    ImGui::Text("Группа 2");

    // Радиокнопки и комбобокс
    static int selected_radio = 0;
    static const char* combo_items[] = { "Строка 1", "Строка 2", "Строка 3" };
    static int combo_current_item = 0;

    if (ImGui::RadioButton("Опция 1", selected_radio == 0)) { selected_radio = 0; combo_current_item = 0; }
    if (ImGui::RadioButton("Опция 2", selected_radio == 1)) { selected_radio = 1; combo_current_item = 1; }
    if (ImGui::RadioButton("Опция 3", selected_radio == 2)) { selected_radio = 2; combo_current_item = 2; }

    ImGui::Combo("Комбобокс", &combo_current_item, combo_items, IM_ARRAYSIZE(combo_items));

    // Обновление радиокнопок при изменении комбобокса
    if (ImGui::IsItemEdited())
    {
        selected_radio = combo_current_item;
    }

    ImGui::EndChild();

    // Третья группа с рамкой
    ImGui::BeginChild("Group 3", ImVec2(800, 200), true);
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

    // Четвертая группа с графиком
    ImGui::BeginChild("Group 4", ImVec2(800, 400), true);
    ImGui::Text("Группа 4");

    static std::vector<float> y_coords;

    // Добавление текущей Y координаты мыши в вектор только если координаты валидны
    if (mouse_valid) {
        y_coords.push_back(io.MousePos.y);
    }

    // Радиокнопки для выбора типа графика
    static int graph_type = 0; // 0 - линии, 1 - гистограмма, 2 - окно последних значений
    ImGui::RadioButton("Линии", &graph_type, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Гистограмма", &graph_type, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Окно последних значений", &graph_type, 2);

    // Слайдер для регулировки ширины окна последних значений
    static int window_size = 100; // Размер окна последних значений
    if (graph_type == 2) {
        ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
    } else {
        ImGui::BeginDisabled();
        ImGui::SliderInt("Ширина окна", &window_size, 1, 1000);
        ImGui::EndDisabled();
    }

    // Отображение графика
    if (graph_type == 0) {
        ImGui::PlotLines("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 1) {
        ImGui::PlotHistogram("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
    } else if (graph_type == 2) {
        if (y_coords.size() > window_size) {
            ImGui::PlotLines("Y координата мыши", &y_coords[y_coords.size() - window_size], window_size, 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        } else {
            ImGui::PlotLines("Y координата мыши", y_coords.data(), y_coords.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(0, 150));
        }
    }

    // Кнопка для очистки графика
    if (ImGui::Button("Очистить график"))
    {
        y_coords.clear();
    }

    ImGui::EndChild();

    // Конец окна
    ImGui::End();
}

int main(int, char**)
{
    // Инициализация GLFW
    if (!glfwInit())
        return -1;

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(1280, 900, "ImGui Demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Загрузка шрифтов
    LoadFonts();

    // Основной цикл
    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents(); // Ожидание событий вместо постоянного опроса

        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Отображение демонстрационного окна
        ShowDemoWindow();

        // Рендеринг
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // Уменьшение частоты обновления
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif 