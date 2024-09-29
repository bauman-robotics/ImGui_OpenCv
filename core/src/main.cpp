
#include "main.h"
#include "main_menu.h"
#include "view_groups.h"
#include "ini_file.h"
#include "tcp-Server.h"
#include "cpu_usage.h"

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
    Select_Mode(CTRL_MODE);

    GLFWwindow *window = nullptr;

    //============================= 

    if (Init_All(&window) == -1) {
        return -1;
    } 
    //=============================

    if (var.cv_mode) {
        
        if (Init_CV() == -1) {
            return -1;
        }
    }      
    //=============================

    Get_CPU_Load_Init(); 
    //Create_Log_File();

    // Основной цикл
    while (!glfwWindowShouldClose(window))
    {
        // Начало нового кадра ImGui
        InitImGuiFrame();

        Menu_Item();
        //====================
        if  ((var.com_port_mode)              &&
             (!var.com_port.init_serial_done) &&
             (!var.com_port.have_to_be_closed)) {
            
            InitSerial();
        }        
        //====================
        if (!var.cv_mode) {
            // Отображение окна "Ctrl"
            ShowCtrlWindow();

            // Ожидание событий вместо постоянного опроса
            // if (var.com_port_mode) { 
            //     glfwPollEvents(); 
            // } else {
            //     glfwWaitEvents();                 
            // }     

            glfwPollEvents();             
        }
        else {
            // Обрабатываем события
            glfwPollEvents();

            int cv_result  = Cv_Processing();

            if (cv_result == -1) {
                break;
            }
        }

        // Рендеринг
        RenderFrame(window); 

        Check_Socket_Connect(); 

        // Уменьшение частоты обновления
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        //std::this_thread::sleep_for(std::chrono::milliseconds(136)); // ~xx FPS        
    }

    // Сохранение состояния ImGui в файл
    ImGui::SaveIniSettingsToDisk(var.io->IniFilename);

    // Очистка ini-файла от неиспользуемых значений
    CleanIniFile(var.io->IniFilename);

    SaveCustomSettings(var.io->IniFilename);
    SaveWindowPosition(window, var.io->IniFilename);

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (var.com_port_mode) {
        CloseSerial();
    }

    if (var.socket.init_socket_done) {

        Socket_Close();
    }
    
    Get_CPU_Load_DeInit();

    return 0;
}
