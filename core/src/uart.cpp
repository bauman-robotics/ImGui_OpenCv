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

#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
using namespace std;

atomic<bool> keep_running;
static atomic<int> packet_count(0);
atomic<int> packets_per_second(0);

static atomic<int> val_data_count(0);
static atomic<int> val_data_per_second(0);

thread serial_thread;
static mutex data_mutex;

static vector<string> serial_data;

static int serial_fd = 0;
condition_variable cvar;

//==========================
static vector<byte> buffer1, buffer2;
static vector<byte>* active_buffer = &buffer1;     // Указатель на активный буфер
static vector<byte>* processing_buffer= &buffer2;  // Указатель на буфер для обработки
static mutex buffer_mutex;
static atomic<bool> data_ready(false);  // Флаг наличия данных для обработки
//==========================


int OpenSerialPort(const char* device);
void ReadSerialData();
void Start_Serial_Thread();
void Stop_Serial_Thread(); // Функция для остановки потока
void UpdatePacketsPerSecond();
void UpdateValDataPerSecond();

double GetPacketsPerSecond();
double Get_Val_Data_PerSecond();
 
vector<int> parseComPortData(const string& prefix);
vector<float> parseComPortData_Float(const string& prefix);
int FindStringIndex(const vector<const char*>& list, const string& target);

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
        cerr << "Ошибка открытия последовательного порта: " << device << endl;
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        cerr << "Ошибка получения атрибутов терминала" << endl;
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
        cerr << "Ошибка установки атрибутов терминала" << endl;
        close(fd);
        return -1;
    }
    cout << "Com port  " << var.com_port.name << "  is open" << endl;

    return fd;
}
//====================================================================

// vector<int> parseComPortData(const string& prefix) {
//     vector<int> results;
//     regex pattern(prefix + R"(\s+(\d+))");
//     cout << "parseComPortData" << endl;
//     for (const auto& data : serial_data) {
//         smatch match;
//         if (regex_search(data, match, pattern)) {
//             results.push_back(stoi(match[1].str()));
//             cout << "push_back" << stoi(match[1].str()) << endl;
//         }
//     }

//     return results;
// }

vector<int> parseComPortData(const string& prefix) {
    vector<int> results;
    regex pattern(prefix + R"(\s+(\d+)*)");

    vector<string> serial_data_buf;
    lock_guard<mutex> lock(data_mutex); // Защита чтения
    serial_data_buf = move(serial_data);
 
    for (const auto& data : serial_data_buf) {
        #ifdef DEBUG_COUT
            cout << "data = " << data << "\n";
        #endif 
        //==================
        auto begin = sregex_iterator(data.begin(), data.end(), pattern);
        auto end = sregex_iterator();
    
        // Поиск всех вхождений префекса данных
        for(auto i = begin; i != end; ++i)  {
            smatch match = *i;
            string prefix_str = match.str();
            #ifdef DEBUG_COUT
                cout << "prefix_str = " << prefix_str << endl;
            #endif 
            // Вытаскиваем значение из каждой подстроки с префиксом.
            smatch match_1_val;
            if (regex_search(prefix_str, match_1_val, pattern)) {
                #ifdef DEBUG_COUT
                    cout << "results =" << stoi(match_1_val[1].str()) << "\n";    
                #endif      
                try {    
                    results.push_back(stoi(match_1_val[1].str())); 
                }  
                catch(...) {
                    cout << "stoi(match_1_val[1].str() Err "  << "\n"; 
                }    
                val_data_count++; 
                #ifdef DEBUG_COUT     
                    cout << "val_data_count = " << val_data_count << endl;  
                #endif      
            }
        }
    }
    return results;
}

//====================================================================

vector<float> parseComPortData_Float(const string& prefix) {

    vector<float> results;
    size_t pos = 0;

    // Локальный буфер для обработки
    vector<byte> local_processing_buffer;

    if (data_ready.load()) {
        {
            lock_guard<mutex> lock(buffer_mutex);
            // Меняем буферы: активный буфер становится обрабатываемым, и наоборот
            swap(active_buffer, processing_buffer);
            local_processing_buffer = move(*processing_buffer); // Перемещаем данные в локальный буфер
            processing_buffer->clear(); // Очищаем буфер для дальнейшего использования
            data_ready.store(false);  // Сбрасываем флаг
        }


        if (var.log.log_Is_Started) {
            
            #ifndef LOG_ONE_VAL_TO_LINE
                Add_Str_To_Log_File_ASCII(local_processing_buffer); 
            #endif 
        }


        //regex pattern(prefix + R"(\s+(\d+))");        // Для int 
        regex pattern(prefix + R"(\s+(\d+(\.\d+)?))");  // Для плавающей точки

        string strData;
        for (const auto& byte : local_processing_buffer) {
            strData += static_cast<char>(byte);  // Явное преобразование byte в char
        }

        #ifdef DEBUG_COUT
            cout << "data = " << strData << "\n";
        #endif

        // Поиск всех вхождений префикса данных
        auto begin = sregex_iterator(strData.begin(), strData.end(), pattern);
        auto end = sregex_iterator();

        for (auto i = begin; i != end; ++i) {
            smatch match = *i;
            string prefix_str = match.str();
            #ifdef DEBUG_COUT
                //cout << "prefix_str = " << prefix_str << endl;
            #endif

            // Вытаскиваем значение из каждой подстроки с префиксом.
            if (match.size() > 1) {
                try {
                    //int value = stoi(match[1].str());
                    float value_f = stof(match[1].str());                    
                    //results.push_back(static_cast<float>(value));  // for log
                    //var.socket.data_f.push_back(static_cast<float>(value));
                    results.push_back(value_f);  // for log
                    var.socket.data_f.push_back(value_f);

                    val_data_count++;
                    #ifdef DEBUG_COUT
                        cout << "results = " << value << "\n";
                        cout << "val_data_count = " << val_data_count << endl;
                    #endif
                } catch (const exception& e) {
                    cerr << "stof(match[1].str()) Err: " << e.what() << "\n";
                }
            }
        }
    }

    return results;
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

double Get_Val_Data_PerSecond() {
    
    return val_data_per_second;
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

void UpdateValDataPerSecond() {
    static int last_count = 0;
    int current_count = val_data_count.load();
    int val_data_this_second = current_count - last_count;
    last_count = current_count;

    val_data_per_second.store(val_data_this_second);
}
//================================================


// Функция для получения списка подключенных портов ttyACM
vector<const char*> getConnectedTTYACMPorts() {
    vector<const char*> ports;
    const string path = "/dev/";
    
    for (const auto& entry : filesystem::directory_iterator(path)) {
        const string filename = entry.path().filename().string();
        if (filename.find("ttyACM") == 0) {
            // Сохраняем строку в статическом векторе для обеспечения времени жизни строки
            static vector<string> static_ports_storage;
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
    serial_thread = thread(ReadSerialData);

    // Запуск потока для обновления количества пакетов в секунду
    thread([]() {
        while (keep_running) {
            this_thread::sleep_for(chrono::seconds(1));
            UpdatePacketsPerSecond();
            UpdateValDataPerSecond();
        }
        packets_per_second.store(0);
        val_data_per_second.store(0);
    }).detach();
}
//================================================

// // Функция для приостановки потока
// void Pause_Serial_Thread() {
//     unique_lock<mutex> lock(mtx);
//     pause_thread = true;
// }
// //================================================

// // Функция для возобновления потока
// void Resume_Serial_Thread() {
//     {
//         unique_lock<mutex> lock(mtx);
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
// void ReadSerialData() {
//     char buf[256];
//     auto start_time = chrono::steady_clock::now();
//     while (keep_running) {

//         // Чтение данных из COM-порта
//         int n = read(serial_fd, buf, sizeof(buf) - 1);
//         if (n > 0) {
//             #ifdef DEBUG_COUT
//                 cout << "___________________________new_Packet" << endl;
//             #endif 
//             buf[n] = '\0';
//             string line(buf);
//             lock_guard<mutex> data_lock(data_mutex);
//             serial_data.push_back(line);
//             packet_count++;
//         } else if (n < 0) {
//             cerr << "Ошибка чтения из последовательного порта" << endl;
//             break;
//         }
//     }
// }


// Функция чтения данных из COM-порта
void ReadSerialData() {
    //char buf[PORT_MSG_BUF_SIZE];
    auto start_time = chrono::steady_clock::now();

    vector<byte> incoming_data(PORT_MSG_BUF_SIZE);  // Убедитесь, что вектор достаточно велик

    while (keep_running) {

        // Чтение данных из COM-порта
        //int bytesRead_n = read(serial_fd, buf, sizeof(buf) - 1);
        int bytesRead_n = read(serial_fd, incoming_data.data(), PORT_MSG_BUF_SIZE);        
        if (bytesRead_n > 0) {
            #ifdef DEBUG_COUT
                cout << "___________________________new_Packet" << endl;
            #endif 

            //buf[bytesRead_n] = '\0';

            // Добавляем считанные данные в активный буфер
            // Защищаем доступ к общему буферу с использованием мьютекса (кратковременная операция)
            {
                lock_guard<mutex> lock(buffer_mutex);
                active_buffer->insert(active_buffer->end(), incoming_data.begin(), incoming_data.begin() + bytesRead_n);
            }
            
            // Уведомление о том, что данные готовы для обработки
            data_ready.store(true);   
            

            packet_count++;
        } else if (bytesRead_n < 0) {
            cerr << "Ошибка чтения из последовательного порта" << endl;
            break;
        }
        //=== Send Cmd ========================================
        if (var.socket.send.need_to_be_sended.load() == 1) {
            var.socket.send.need_to_be_sended.store(0);

            ssize_t bytesSent = write(serial_fd, var.socket.send.message, strlen(var.socket.send.message));
            if (bytesSent < 0) {
                perror("Ошибка отправки данных клиенту");
            } else {
                cout << "send to Client: " << var.socket.send.message << endl;
            }      
        }

        //=====================================================
    }
}
//================================================

// Функция для поиска строки в списке
int FindStringIndex(const vector<const char*>& list, const string& target) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (string(list[i]) == target) { // Преобразование const char* в string для сравнения
            return static_cast<int>(i); // Возвращаем индекс найденной строки
        }
    }
    return -1; // Возвращаем -1, если строка не найдена
}

// for float data 
vector<float> parseBinary_Com_PortData_Float() {
    // active_buffer
    // processing_buffer
    // local_processing_buffer
    //==== output ====
    // var.socket.data_f 
    // results -- only for logs 

    const uint16_t HEADER_SIZE = 4;
    vector<float> results;
    size_t pos = 0;

    // Локальный буфер для обработки
    vector<byte> local_processing_buffer;

    if (data_ready.load()) {
        {
            lock_guard<mutex> lock(buffer_mutex);
            // Меняем буферы: активный буфер становится обрабатываемым, и наоборот
            swap(active_buffer, processing_buffer);
            local_processing_buffer = move(*processing_buffer); // Перемещаем данные в локальный буфер
            processing_buffer->clear(); // Очищаем буфер для дальнейшего использования
            data_ready.store(false);  // Сбрасываем флаг
        }

        // Теперь данные находятся в локальном буфере, и мы можем их обрабатывать без мьютекса
        if (!local_processing_buffer.empty()) {
            while (pos < local_processing_buffer.size()) {
                // Читаем заголовок пакета
                if (local_processing_buffer.size() - pos < HEADER_SIZE) {
                    break;  // Недостаточно данных для чтения заголовка
                }

                BinPacketHeader header;

                // Читаем первые 2 байта для типа пакета
                header.type.low = static_cast<uint8_t>(local_processing_buffer.at(pos));
                pos++;
                header.type.hi = static_cast<uint8_t>(local_processing_buffer.at(pos));
                pos++;
                header.type.val = (static_cast<uint16_t>(header.type.hi) << 8) | header.type.low;

                // Читаем следующие 2 байта для полного размера пакета
                header.full_packet_size.low = static_cast<uint8_t>(local_processing_buffer.at(pos));
                pos++;
                header.full_packet_size.hi = static_cast<uint8_t>(local_processing_buffer.at(pos));
                pos++;
                header.full_packet_size.val = (static_cast<uint16_t>(header.full_packet_size.hi) << 8) | header.full_packet_size.low;

                // Проверяем, совпадает ли тип пакета с целевым
                // === Если данные в формате int ==============================                    
                if (header.type.val == BYNARY_PACKET_INT_KEY) {


                    // if (header.full_packet_size.val < HEADER_SIZE || 
                    //     pos + header.full_packet_size.val > processing_buffer->size()) {
                    //     // Недостаточно данных или некорректный размер пакета
                    //     break;
                    // }

                    // Проверяем, хватает ли данных в векторе для чтения тела пакета
                    if (pos + (header.full_packet_size.val - HEADER_SIZE) <= local_processing_buffer.size()) {
                        // Читаем тело пакета и добавляем его в выходной вектор
                        for (size_t i = 0; i < (header.full_packet_size.val - HEADER_SIZE); i += 2) {
                            int16_t value = (static_cast<int16_t>(local_processing_buffer.at(pos + i + 1)) << 8)
                                            | static_cast<int16_t>(local_processing_buffer.at(pos + i));
                            results.push_back(static_cast<float>(value));  // for log
                            var.socket.data_f.push_back(static_cast<float>(value));
                            val_data_count++;
                        }
                        pos += (header.full_packet_size.val - HEADER_SIZE);
                    } else {
                        // Недостаточно данных для чтения полного пакета
                        break;
                    }
                // === Если данные в формате float ==============================    
                } else if (header.type.val == BYNARY_PACKET_FLOAT_KEY) {
                    // Проверяем, хватает ли данных в векторе для чтения тела пакета
                    if (pos + (header.full_packet_size.val - HEADER_SIZE) <= local_processing_buffer.size()) {
                        // Читаем тело пакета и добавляем его в выходной вектор
                        for (size_t i = 0; i < (header.full_packet_size.val - HEADER_SIZE); i += sizeof(float)) {
                            // Копируем байты float из буфера
                            float value;
                            std::memcpy(&value, &local_processing_buffer[pos + i], sizeof(float));
                            
                            results.push_back(value);  // for log
                            var.socket.data_f.push_back(value);
                            val_data_count++;
                        }
                        pos += (header.full_packet_size.val - HEADER_SIZE);
                    } else {
                        // Недостаточно данных для чтения полного пакета
                        break;
                    }      
                }  else {
                    // Пропускаем пакет с неподходящим типом
                    pos += header.full_packet_size.val - HEADER_SIZE;
                }
            }
        }
    }

    return results;
}
//===========================================================================================
