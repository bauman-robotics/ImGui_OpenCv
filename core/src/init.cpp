#include "main.h"
#include <filesystem> // Для работы с путями
#include "ini_file.h"

// Прототипы функций
int InitGLFWAndImGui(GLFWwindow** window);
void CreateTexture();
void InitFrameSize(int size); 
void Close_CV(); 

//=========================================================================
int Init_All(GLFWwindow** window) {



    if (InitGLFWAndImGui(window) != 0) {
        return -1;
    }

    LoadFonts();

    var.cv.playing = 1;
    var.Init_CV_done = 0;
    var.com_port.init_serial_done = 0;

    var.cv.frame_size = FRAME_SIZE_640;

    var.com_port.name = "/dev/ttyACM0";

    var.com_port.data_prefix = "data";

    var.com_port.i_baud_rate = 115200;
    
    LoadCustomSettings(var.io->IniFilename);

    if (var.com_port_mode) {

        if (InitSerial() < 0) {
            //return -1;
            //Select_Mode(CTRL_MODE);
        }
    }
    return 0;
}
//=========================================================================

int Init_CV() {
    int dev_id = 0;

    var.cv.cap.open(dev_id);
    if (!var.cv.cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    InitFrameSize(var.cv.frame_size);

    CreateTexture();
    var.Init_CV_done = 1;
    return 0;
}

//=========================================================================

void Close_CV() {
    if (var.cv.cap.isOpened()) {
        var.cv.cap.release();
    }

    if (var.cv.texture != 0) {
        glDeleteTextures(1, &var.cv.texture);
        var.cv.texture = 0;
    }

    var.Init_CV_done = 0;
}
//=========================================================================

int InitGLFWAndImGui(GLFWwindow** window) {
    if (!glfwInit()) {
        return -1;
    }

    // Установить флаг для запрета изменения размера окна
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

   int win_width = 880;

    *window = glfwCreateWindow(win_width, 736, "ImGui Demo", NULL, NULL);
    if (!*window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;

    var.io = &ImGui::GetIO();  // Инициализация указателя ImGuiIO

    // Указать путь к файлу imgui.ini
    static std::filesystem::path ini_path = std::filesystem::current_path() / "imgui.ini";
    var.io->IniFilename = ini_path.c_str();
    std::cout << "IniFilename set to: " << var.io->IniFilename << std::endl;

    // Загрузка позиции окна
    LoadWindowPosition(*window, var.io->IniFilename);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return 0;
}
//=========================================================================

void CreateTexture() {
    glGenTextures(1, &var.cv.texture);
    glBindTexture(GL_TEXTURE_2D, var.cv.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}
//=========================================================================


void InitFrameSize(int size) {

    // // Устанавливаем разрешение видеопотока
    int width = 640;
    int height = 480;


    switch (size) {
        //==================
        case (FRAME_SIZE_MIN): 
            width = 320;
            height = 180;        
        break;
        //==================
        case (FRAME_SIZE_640): 
            width = 640;
            height = 480;
        break;
        //==================
        case (FRAME_SIZE_MAX): 
            width = 1280;
            height = 720;  
        break;        
        //==================
        default: 
            width = 640;
            height = 480;
        break;
    }


    var.cv.cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    var.cv.cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    // Проверяем установленные значения
    double actualWidth = var.cv.cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double actualHeight = var.cv.cap.get(cv::CAP_PROP_FRAME_HEIGHT);
}

//================================================

void Select_Mode(int mode){
    switch (mode){
    case (CTRL_MODE):
        var.ctrl_mode     = true;
        var.com_port_mode = false;
        var.cv_mode       = false;
    break;
    case (COM_PORT_MODE):
        var.ctrl_mode     = false;
        var.com_port_mode = true;
        var.cv_mode       = false;
    break;
    case (OPENCV_MODE):
        var.ctrl_mode     = false;
        var.com_port_mode = false;
        var.cv_mode       = true;
    break;
    default:
        var.ctrl_mode     = true;
        var.com_port_mode = false;
        var.cv_mode       = false;
    break;
    }
} 

//================================================

int Get_Mode() {
    if (var.ctrl_mode) {
        return CTRL_MODE;
    }
    if (var.com_port_mode) {
        return COM_PORT_MODE;
    }
    if (var.cv_mode) {
        return OPENCV_MODE;
    }

    return -1;
} 