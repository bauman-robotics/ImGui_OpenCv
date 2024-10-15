#include "main.h"
#include <unordered_set>
#include <set>
#include <cstdint>
#include <stdexcept>
#include "defines.h"

std::set<std::string> active_windows = { "[MainWindow]", "[CustomSettings]" };

void Keys_Parser(std::string key, std::string value); 
uint32_t stringToUint32(const std::string& str);
void Set_Signal_Type_Flags(int signal_type); 
//==================================================================================

void SaveCustomSettings(const char* filename) {
    std::ofstream file(filename, std::ios::app); // Открываем файл в режиме добавления

    char buf_mode[20] = {0}; // 
    sprintf(buf_mode, "Mode=%d\n", Get_Mode()); // Используем \n для переноса строки

    char buf_port[20] = {0}; // 
    sprintf(buf_port, "Com-port=%s\n", var.com_port.name.c_str()); // Используем \n для переноса строки

    char buf_prefix[20] = {0}; // 
    sprintf(buf_prefix, "data_prefix=%s\n", var.com_port.data_prefix.c_str()); // Используем \n для переноса строки

    // char buf_baud_rate[20] = {0}; // 
    // sprintf(buf_baud_rate, "baud_rate=%d\n", var.com_port.i_baud_rate); // Используем \n для переноса строки

    char buf_auto_open[20] = {0}; // 
    if (var.com_port_mode) {
        sprintf(buf_auto_open, "auto_open=%d\n", (int)var.com_port.init_serial_done); // Используем \n для переноса строки
    } else if (var.ctrl_mode) {
        sprintf(buf_auto_open, "auto_open=%d\n", (int)var.socket.init_socket_done); // Используем \n для переноса строки
    } else {
        sprintf(buf_auto_open, "auto_open=%d\n", 0);
    }

    char buf_signal_type[20] = {0}; // 
    sprintf(buf_signal_type, "signal_type=%d\n", var.socket.ina226.mode.signal_type); // Используем \n для переноса строки

    if (file.is_open()) {
        file << "[CustomSettings]\n";
        file << buf_mode;
        file << buf_port;
        //file << buf_baud_rate; 
        file << buf_prefix;       
        file << buf_auto_open;
        file << buf_signal_type;

        file.close();
    }
}
//==================================================================================

void LoadCustomSettings(const char* filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line == "[CustomSettings]") {
                while (std::getline(file, line) && !line.empty()) {
                    auto pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = line.substr(0, pos);
                        std::string value = line.substr(pos + 1);
                        std::cout << "Loaded custom setting: " << key << " = " << value << std::endl;
                        Keys_Parser(key, value);
                    }
                }
            }
        }
        file.close();
    }
}
//==================================================================================

void Keys_Parser(std::string key, std::string value) {
    //================================    
    if (key == "Mode") {
        if (value == "0") {
            Select_Mode(CTRL_MODE);
        }        
        else if (value == "1") {
            Select_Mode(COM_PORT_MODE);
        }
        else if (value == "2") {
            Select_Mode(OPENCV_MODE);
        }
        else {
            Select_Mode(POST_REQUEST_MODE);
        }       
    }
    //================================
    if (key == "Com-port") {
        var.com_port.last_port_name = value;
    }
    //================================   
    if (key == "data_prefix") {
        var.com_port.data_prefix = value;
        var.socket.data_prefix = value;        
    }
    //================================   
    // if (key == "baud_rate") {
    //     uint32_t i_value = 115200;
    //     try {
    //         i_value = stringToUint32(value);
    //         std::cout << "Converted value: " << value << std::endl;
    //     } catch (const std::exception& e) {
    //         std::cerr << "Conversion failed: " << e.what() << std::endl;
    //     }

    //     var.com_port.i_baud_rate = i_value;
    // }
    //================================   
    if (key == "auto_open") {
        uint32_t i_value = 0;
        try {
            i_value = stringToUint32(value);
            std::cout << "Converted value: " << value << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Conversion failed: " << e.what() << std::endl;
        }
        var.com_port.auto_open = (bool)i_value;  
    }      
    //================================   
    if (key == "signal_type") {
        uint32_t i_value = 0;
        try {
            i_value = stringToUint32(value);
            std::cout << "Converted value: " << value << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Conversion failed: " << e.what() << std::endl;
        }
        var.socket.ina226.mode.signal_type = (int)i_value;  

        Set_Signal_Type_Flags(var.socket.ina226.mode.signal_type);

    }        
    
    //================================ 
}
//==================================================================================

void SaveWindowPosition(GLFWwindow* window, const char* ini_filename) {
    int xpos, ypos;
    glfwGetWindowPos(window, &xpos, &ypos);
    
    std::ofstream file(ini_filename, std::ios::app); // Открываем файл в режиме добавления
    if (file.is_open()) {
        file << "\n";
        file << "[MainWindow]\n";
        file << "PosX=" << xpos << "\n";
        file << "PosY=" << ypos << "\n";
        file.close();
    }
}
//==================================================================================

void LoadWindowPosition(GLFWwindow* window, const char* ini_filename) {
    std::ifstream file(ini_filename);
    if (file.is_open()) {
        std::string line;
        int xpos = -1, ypos = -1;
        while (std::getline(file, line)) {
            if (line == "[MainWindow]") {
                while (std::getline(file, line) && !line.empty()) {
                    auto pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = line.substr(0, pos);
                        int value = std::stoi(line.substr(pos + 1));
                        if (key == "PosX") xpos = value;
                        else if (key == "PosY") ypos = value;
                    }
                }
                break;
            }
        }
        file.close();
        if (xpos != -1 && ypos != -1) {
            glfwSetWindowPos(window, xpos, ypos);
        }
    }
}
//==================================================================================

void CleanIniFile(const char* ini_filename) {
    std::ifstream file(ini_filename);
    if (!file.is_open()) {
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string line;
    std::string current_section;
    std::stringstream new_content;

    while (std::getline(buffer, line)) {
        if (!line.empty() && line[0] == '[') {
            current_section = line;
        }

        if (current_section.empty() || active_windows.find(current_section) != active_windows.end()) {
            new_content << line << "\n";
        }
    }

    std::ofstream out_file(ini_filename);
    if (out_file.is_open()) {
        out_file << new_content.str();
        out_file.close();
    }
}
//==================================================================================

uint32_t stringToUint32(const std::string& str) {
    try {
        // std::stoul может бросить исключение std::invalid_argument или std::out_of_range
        unsigned long value = std::stoul(str);

        // Проверка на переполнение uint32_t
        if (value > std::numeric_limits<uint32_t>::max()) {
            throw std::out_of_range("Value is out of range for uint32_t");
        }

        return static_cast<uint32_t>(value);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        throw;
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
        throw;
    }
}
//==================================================================================

void Set_Signal_Type_Flags(int signal_type) {

    switch (signal_type) {
        case 0: 
            var.socket.ina226.mode.voltage = 0;
            var.socket.ina226.mode.current = 0;
            var.socket.ina226.mode.power   = 0; 
        break;
        
        case 1: 
            var.socket.ina226.mode.voltage = 0;
            var.socket.ina226.mode.current = 0;
            var.socket.ina226.mode.power   = 0; 
        break;
        case 2:
            var.socket.ina226.mode.voltage = 1;
            var.socket.ina226.mode.current = 0;
            var.socket.ina226.mode.power   = 1;  
        break;
        case 3:
            var.socket.ina226.mode.voltage = 0;
            var.socket.ina226.mode.current = 1;
            var.socket.ina226.mode.power   = 0; 

        break;
        case 4:
            var.socket.ina226.mode.voltage = 0;
            var.socket.ina226.mode.current = 0;
            var.socket.ina226.mode.power   = 1;              
        break;                                                
    }

}