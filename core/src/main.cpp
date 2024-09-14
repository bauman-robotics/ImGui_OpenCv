
#include "main.h"
//============================================================================

void InitImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
//============================================================================

void RenderFrame(GLFWwindow* window) {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
//============================================================================

int main(int, char**)
{  
    
    GLFWwindow *window = nullptr;
    GLuint texture;   
    
    //============================= 
    #ifdef USE_OPENCV
        cv::VideoCapture cap(0);
        int init_result = Init_All_CV(cap, &texture, &window);
    #else 
        int init_result = Init_All(&window);
    #endif 

    if (init_result == -1) {
        return -1;
    } 
    //=============================

    // Основной цикл
    while (!glfwWindowShouldClose(window))
    {
        // Начало нового кадра ImGui
        InitImGuiFrame();

        #ifndef USE_OPENCV  
            // Отображение демонстрационного окна
            ShowDemoWindow();

            // Ожидание событий вместо постоянного опроса
            #ifdef USE_COM_PORT  
                glfwPollEvents(); 
            #else 
                glfwWaitEvents();                 
            #endif       
        #else
            // Обрабатываем события
            glfwPollEvents();

            int cv_result = Cv_Processing(cap, &texture);
            if (cv_result == -1) {
                break;
            }
        #endif 

        // Рендеринг
        RenderFrame(window); 

        // Уменьшение частоты обновления
        //std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    #ifdef USE_COM_PORT
        CloseSerial();
    #endif 

    return 0;
}

