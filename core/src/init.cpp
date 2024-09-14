    #include "main.h"

// Прототипы функций
int InitGLFWAndImGui(GLFWwindow** window);
void CreateTexture(GLuint* texture);

//=========================================================================
int Init_All(int* serial_fd, GLFWwindow** window) {
    #if defined(USE_COM_PORT)
    *serial_fd = OpenSerialPort("/dev/ttyACM0");
    if (*serial_fd < 0) {
        return -1;
    }
    #endif

    if (InitGLFWAndImGui(window) != 0) {
        return -1;
    }

    LoadFonts();
    return 0;
}
//=========================================================================

int Init_All_CV(cv::VideoCapture& cap, GLuint* texture, GLFWwindow** window) {
    #if defined(USE_COM_PORT) && defined(USE_OPENCV)
    return -1;
    #endif

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    if (InitGLFWAndImGui(window) != 0) {
        return -1;
    }

    CreateTexture(texture);
    LoadFonts();
    return 0;
}
//=========================================================================

int InitGLFWAndImGui(GLFWwindow** window) {
    if (!glfwInit()) {
        return -1;
    }
    int win_width = 1280;
    #ifndef USE_OPENCV 
        win_width = 880;
    #endif 

    *window = glfwCreateWindow(win_width, 720, "ImGui Demo", NULL, NULL);
    if (!*window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return 0;
}
//=========================================================================

void CreateTexture(GLuint* texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}