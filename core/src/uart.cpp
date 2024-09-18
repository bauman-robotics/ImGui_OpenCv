#include "main.h"
#include "uart.h"
#include "view_groups.h"
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
#include <filesystem>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::mutex data_mutex;

std::atomic<bool> keep_running;
static int serial_fd = 0;
std::thread serial_thread;
std::condition_variable cvar;
static std::vector<std::string> serial_data;
static std::atomic<int> packet_count(0);
std::atomic<int> packets_per_second(0);

int OpenSerialPort(const char* device);
void ReadSerialData();
void Start_Serial_Thread();
void UpdatePacketsPerSecond();
double GetPacketsPerSecond();

int FindStringIndex(const std::vector<const char*>& list, const std::string& target);

//====================================================================
int InitSerial() {

    var.com_port.ports_list = getConnectedTTYACMPorts();

    // if  ((var.com_port.selected_port == "") &&
    //     (!var.com_port.ports_list.empty())) {
    //     var.com_port.selected_port = var.com_port.ports_list[0];

    //     int index_last_port = FindStringIndex(var.com_port.ports_list, var.com_port.last_port_name);
    
    //     if (index_last_port != -1) {
    //         var.com_port.selected_port = var.com_port.last_port_name;
    //         var.com_port.selected_port_index = index_last_port;
    //         var.com_port.name = var.com_port.last_port_name;
    //     }
    // }

    if  ((var.com_port.selected_port == "") &&
         (!var.com_port.ports_list.empty())) {
         var.com_port.selected_port = var.com_port.ports_list[0];
    }

    serial_fd = OpenSerialPort(var.com_port.selected_port.c_str()); 

    if (serial_fd < 0) {
        keep_running = false;
        var.com_port.init_serial_done = false;
        var.com_port.have_to_be_closed = true;
        return -1;
    }
    Start_Serial_Thread();

    Clear_Plot_Data();

    keep_running = true;
    var.com_port.init_serial_done = true;
    var.com_port.have_to_be_closed = false;
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
    std::cout << "Com port  " << var.com_port.name << "  is open" << std::endl;

    return fd;
}
//====================================================================

std::vector<int> parseComPortData(const std::string& prefix) {
    std::vector<int> results;
    std::regex pattern(prefix + R"(\s+(\d+))");

    for (const auto& data : serial_data) {
        std::smatch match;
        if (std::regex_search(data, match, pattern)) {
            results.push_back(std::stoi(match[1].str()));
        }
    }

    return results;
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
    var.com_port.init_serial_done = false;
    var.com_port.have_to_be_closed = true;
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

// Функция для получения списка подключенных портов ttyACM
std::vector<const char*> getConnectedTTYACMPorts() {
    std::vector<const char*> ports;
    const std::string path = "/dev/";
    
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const std::string filename = entry.path().filename().string();
        if (filename.find("ttyACM") == 0) {
            // Сохраняем строку в статическом векторе для обеспечения времени жизни строки
            static std::vector<std::string> static_ports_storage;
            static_ports_storage.push_back(entry.path().string());
            ports.push_back(static_ports_storage.back().c_str());
        }
    }
    
    return ports;
}
//================================================

// Функция для запуска потока
void Start_Serial_Thread() {
    keep_running  = true;
    //pause_thread = false;

    // Запуск потока для чтения данных из COM-порта
    serial_thread = std::thread(ReadSerialData);

    // Запуск потока для обновления количества пакетов в секунду
    std::thread([]() {
        while (keep_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            UpdatePacketsPerSecond();
        }
        packets_per_second.store(0);
    }).detach();
}
//================================================

// // Функция для приостановки потока
// void Pause_Serial_Thread() {
//     std::unique_lock<std::mutex> lock(mtx);
//     pause_thread = true;
// }
// //================================================

// // Функция для возобновления потока
// void Resume_Serial_Thread() {
//     {
//         std::unique_lock<std::mutex> lock(mtx);
//         pause_thread = false;
//     }
//     cvar.notify_all();
// }
//================================================

// Функция для остановки потока
void Stop_Serial_Thread() {
    keep_running = false;
    //Resume_Serial_Thread(); // На случай, если поток приостановлен
    if (serial_thread.joinable()) {
        serial_thread.join();
    }
}
//================================================

// Функция чтения данных из COM-порта
void ReadSerialData() {
    char buf[256];
    auto start_time = std::chrono::steady_clock::now();
    while (keep_running) {

        // Чтение данных из COM-порта
        int n = read(serial_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            std::string line(buf);
            std::lock_guard<std::mutex> data_lock(data_mutex);
            serial_data.push_back(line);
            packet_count++;
        } else if (n < 0) {
            std::cerr << "Ошибка чтения из последовательного порта" << std::endl;
            break;
        }
    }
}
//================================================

// Функция для поиска строки в списке
int FindStringIndex(const std::vector<const char*>& list, const std::string& target) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (std::string(list[i]) == target) { // Преобразование const char* в std::string для сравнения
            return static_cast<int>(i); // Возвращаем индекс найденной строки
        }
    }
    return -1; // Возвращаем -1, если строка не найдена
}