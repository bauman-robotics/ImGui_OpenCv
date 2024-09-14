#include "main.h"
#include "uart.h"
//============================
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>  // Для функции read
#include <fcntl.h>   // Для функции open
#include <termios.h> // Для работы с последовательным портом
#include <thread>    // Для работы с потоками
//============================
#include <mutex>
#include <atomic>
//============================

std::mutex data_mutex;
std::atomic<bool> keep_running(true);

static int serial_fd = 0;
std::thread serial_thread;
static std::vector<std::string> serial_data;
static std::atomic<int> packet_count(0);
std::atomic<int> packets_per_second(0);

int OpenSerialPort(const char* device);
void ReadSerialData();
void Start_Serial_Thread();
void UpdatePacketsPerSecond();
double GetPacketsPerSecond();


//====================================================================
int InitSerial() {

    serial_fd = OpenSerialPort("/dev/ttyACM0");
    if (serial_fd < 0) {
        return -1;
    }
    Start_Serial_Thread();
    return 0;
}
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

std::vector<int> parseComPortData() {
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

void ReadSerialData() {
    char buf[256];
    auto start_time = std::chrono::steady_clock::now();
    while (keep_running) {
        int n = read(serial_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            std::string line(buf);
            std::lock_guard<std::mutex> lock(data_mutex);
            serial_data.push_back(line);
            packet_count++;
        } else if (n < 0) {
            std::cerr << "Ошибка чтения из последовательного порта" << std::endl;
            break;
        }
    }
}
//================================================

void Start_Serial_Thread() {
    keep_running = true;
    // Запуск потока для чтения данных из COM-порта
    serial_thread = std::thread(ReadSerialData);

    // Запуск потока для обновления количества пакетов в секунду
    std::thread([]() {
        while (keep_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            UpdatePacketsPerSecond();
        }
    }).detach();
}
//================================================

void ClearSerialData() {
    std::lock_guard<std::mutex> lock(data_mutex);
    serial_data.clear();
}
//================================================

void CloseSerial() {
    // Завершение потока
    keep_running = false;
    if (serial_thread.joinable()) {
        serial_thread.join();
    }
    close(serial_fd);
}
//================================================

double GetPacketsPerSecond() {
    
    return packets_per_second;
}
//================================================

void UpdatePacketsPerSecond() {
    static int last_count = 0;
    int current_count = packet_count.load();
    int packets_this_second = current_count - last_count;
    last_count = current_count;

    packets_per_second.store(packets_this_second);
}
//================================================
