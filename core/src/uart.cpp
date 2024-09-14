#include "main.h"
#include "uart.h"
//============================
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>  // Для функции read
//============================

int OpenSerialPort(const char* device);
void ReadSerialData(int fd, std::vector<std::string>& serial_data);
int ExtractDataValue(const std::string& input);
void CleanSerialData(std::string& data);

//====================================================================

int OpenSerialPort(const char* device)
{
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        std::cerr << "Ошибка открытия последовательного порта: " << device << std::endl;
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        std::cerr << "Ошибка получения атрибутов терминала" << std::endl;
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-битные символы
    tty.c_iflag &= ~IGNBRK;                     // отключить игнорирование BREAK
    tty.c_lflag = 0;                            // без канонического режима, без эхо
    tty.c_oflag = 0;                            // без обработки вывода
    tty.c_cc[VMIN] = 1;                         // читать блокирующе
    tty.c_cc[VTIME] = 0;                        // таймаут не нужен

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // отключить управление потоком
    tty.c_cflag |= (CLOCAL | CREAD);            // включить приемник
    tty.c_cflag &= ~(PARENB | PARODD);          // отключить проверку четности
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        std::cerr << "Ошибка установки атрибутов терминала" << std::endl;
        close(fd);
        return -1;
    }

    return fd;
}
//====================================================================

// void CleanSerialData(std::string& data)
// {
//     // Удаление управляющих символов
//     data.erase(std::remove_if(data.begin(), data.end(), [](char c) {
//         return !isprint(c) && !isspace(c);
//     }), data.end());
// }
//====================================================================

// int ExtractDataValue(const std::string& input) {
//     std::string keyword = "data ";
//     size_t pos = input.find(keyword);
    
//     if (pos != std::string::npos) {
//         pos += keyword.length();
//         std::istringstream iss(input.substr(pos));
//         int value;
//         if (iss >> value) {
//             return value;
//         }
//     }
    
//     // Если ключевое слово не найдено или после него нет числа, возвращаем -1 или другое значение по умолчанию
//     return -1;
// }

//====================================================================

void ReadSerialData(int fd, std::vector<std::string>& serial_data)
{
    char buf[256];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0)
    {
        buf[n] = '\0';
        std::string line(buf);
        serial_data.push_back(line);
    }
}

//================================================

std::vector<int> parseComPortData(const std::vector<std::string>& serial_data) {
    std::vector<int> results;
    std::regex pattern(R"(data (\d+))");

    for (const auto& data : serial_data) {
        std::smatch match;
        if (std::regex_search(data, match, pattern)) {
            results.push_back(std::stoi(match[1].str()));
        }
    }

    return results;
}
//================================================
