#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

#include <thread>    // Для работы с потоками
#include <atomic>

#include "variables.h"
#include <regex>
#include <chrono>
#include "log_file.h"
#include "defines.h"

using namespace std;

static atomic<int> is_binding(0);
static atomic<bool> need_to_reopen_socket_port(0);
static atomic<bool> state_wait_client(0);
static atomic<bool> keep_running(0);
static atomic<int> packet_count(0);
static atomic<int> packets_per_second(0);

static atomic<int> val_data_count(0);
static atomic<int> val_data_per_second(0);

static thread binding_thread;
static thread socket_thread;
static thread wait_client_thread;
static mutex data_mutex;

// static vector<string> socket_data;

static vector<byte> socket_data;

static byte msg[SOCKET_MSG_BUF_SIZE];
static int newSd;
static int serverSd = 0;
static int bytesRead = 0;
static int bytesRead_n = 0;
static int bytesWritten = 0;
static struct timeval start1, end1;


int Socket_Server_Init(int port);
int Port_Open(int port);
void Start_Server_Thread(); 
void Stop_Socket_Thread(); 
void ReadSocketData();
void UpdateSocketPacketsPerSecond();
void UpdateSocketValDataPerSecond();
int Socket_Close();
double GetPacketsPerSecond_S();
double Get_Val_Data_PerSecond_S();

void Check_Socket_Connect();

void Wait_Socket_Client(); // Потоковая функция ожидание подключения клиента 

vector<int> parseSocketData(const string& prefix);
vector<int> parseBinarySocketData(); 

sockaddr_in newSockAddr;
socklen_t newSockAddrSize = sizeof(newSockAddr);


//================================================

int Socket_Server_Init(int port) {
 
    if (!var.socket.init_socket_done) {

        if (Port_Open(port) == -1) {

            return -1;
        }

        // state_wait_client = 1;


        // if (wait_client_thread.joinable()) {
        //     wait_client_thread.join();
        // }
        
        // // Запуск потока ожидания подключения клиента         
        // wait_client_thread = thread(Wait_Socket_Client);

        // var.socket.init_socket_done = 1;
    } else {
        keep_running  = true;
    }

    return 0;   
}
//================================================
sockaddr_in servAddr;
int opt = 1;

int Port_Open(int port) {
    //setup a socket and connection tools
    //sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    //serverSd = socket(AF_INET, SOCK_DGRAM, 0);

    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }

    cout << "serverSd = " << serverSd << endl;
    //bind the socket to its local address

    // Устанавливаем опцию SO_REUSEADDR для повторного использования порта
    if (setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(serverSd);
        exit(EXIT_FAILURE);
    }

    var.socket.have_to_be_binded = 1;

    // int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    // if(bindStatus < 0)
    // {
    //     cerr << "Error binding socket to local address" << endl;
    //     //exit(0);
    //     return -1;
    // }
    // cout << "Waiting for a client to connect..." << endl;
    // //listen for up to 5 requests at a time
    // listen(serverSd, 5);  

    return 0;
}
//================================================

void Wait_Socket_Client() {

    while (state_wait_client) {

        // Принятие входящего соединения
        newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);

        if (newSd  < 0) {
            perror("Ошибка при принятии соединения");
            continue;
        }

        cout << "Принято новое соединение" << endl;
        cout << "newSd=" << newSd << endl;   

        // Запуск потока приема данных 
        //if (!keep_running) {

       
        Start_Server_Thread();   
        state_wait_client = 0;  

       // }
        gettimeofday(&start1, NULL);
        //also keep track of the amount of data sent as well     
    }
}
//================================================

// Функция для запуска потока
void Start_Server_Thread() {

    keep_running  = true;

    // Запуск потока для чтения данных из Socket
    socket_thread = thread(ReadSocketData);

    // Запуск потока для обновления количества пакетов в секунду
    thread([]() {
        while (keep_running) {
            this_thread::sleep_for(chrono::seconds(1));
            UpdateSocketPacketsPerSecond();
            UpdateSocketValDataPerSecond();

            cout << "parser_data_size " << var.debug.parser_data_size.load() << endl;
            cout << "new_parser_data_size " << var.debug.new_parser_data_size.load() << endl;
        }
        packets_per_second.store(0);
        val_data_per_second.store(0);
    }).detach();
}
//================================================

// Функция для остановки потока
void Stop_Socket_Thread() {
    keep_running = false;
    //Resume_Serial_Thread(); // На случай, если поток приостановлен
    if (socket_thread.joinable()) {
        socket_thread.join();
    }
}
//================================================

// Функция чтения данных из Socket
void ReadSocketData() {
    //char buf[256];
    auto start_time = chrono::steady_clock::now();

    fd_set readfds;
    struct timeval timeout;

    while(keep_running)   {

        FD_ZERO(&readfds);
        FD_SET(newSd, &readfds);

        // Установка таймаута в 5 секунд
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // Инициализация вектора для хранения данных
        static vector<byte> data;

        int activity = select(newSd + 1, &readfds, NULL, NULL, &timeout);
        //cout << "activity =" << activity<< endl;
        if (activity < 0) {
            perror("Ошибка при вызове select");
            close(newSd);
            state_wait_client = 1;  // Сброс состояния ожидания клиента
            keep_running = 0;
            need_to_reopen_socket_port = 1;            
            break;
        } else if (activity == 0) {
            cout << "Таймаут: клиент неактивен" << endl;
            close(newSd);
            state_wait_client = 1;  // Сброс состояния ожидания клиента
            keep_running = 0;
            need_to_reopen_socket_port = 1;
            break;
        } else {
            if (FD_ISSET(newSd, &readfds)) {
                // Чтение данных от клиента

                bytesRead_n = recv(newSd, (char*)&msg, sizeof(msg), 0);
                
                bytesRead +=bytesRead_n; 

                #ifdef DEBUG_COUT
                    cout << "Received " << bytesRead_n << " Bytes" << endl;
                #endif 

                //==== Отправка данных клиенту ====
                if (var.socket.send.need_to_be_sended.load() == 1) {
                    var.socket.send.need_to_be_sended.store(0);
                    send(newSd, var.socket.send.message, strlen(var.socket.send.message), 0);
                    cout << "send to Client: " << var.socket.send.message << endl;
                }
                //=================================

                // Чтение данных
                if (bytesRead_n > 0) {
                    lock_guard<mutex> data_lock(data_mutex);
 
                    if ((var.log.log_Is_Started) && (!var.socket.hex_receive)) {
                        Add_Str_To_Log_File((uint8_t *)&msg, bytesRead_n);
                    }
                    socket_data.insert(socket_data.end(), msg, msg + bytesRead_n);
                    packet_count++;
                    #ifdef DEBUG_COUT
                        cout << "packet_count____________________________________" << packet_count << endl;
                    #endif 
                } else if (bytesRead_n == 0) {
                    cout << "Клиент закрыл соединение" << endl;
                    close(newSd);
                    state_wait_client = 1;  
                    keep_running = 0;
                    need_to_reopen_socket_port = 1;
                    break;
                } else {
                    cerr << "Ошибка чтения из сокета" << endl;
                    break;
                }
            }
        }          
    }
}
//====================================================================

vector<int> parseSocketData(const string& prefix) {
    lock_guard<mutex> lock(data_mutex);  // Защита чтения

    // Проверяем, что буфер не пустой
    if (socket_data.empty()) {
        #ifdef DEBUG_COUT
            //cout << "socket_data is empty, returning empty results.\n";
        #endif
        return {};  // Возвращаем пустой вектор
    }

    // Перемещаем данные из socket_data в socket_data_buf
    vector<byte> socket_data_buf = move(socket_data);

    // Очищаем оригинальный буфер
    socket_data.clear();

    vector<int> results;
    regex pattern(prefix + R"(\s+(\d+))");

    string strData;
    for (const auto& byte : socket_data_buf) {
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
                int value = stoi(match[1].str());
                results.push_back(value);
                val_data_count++;
                #ifdef DEBUG_COUT
                    cout << "results = " << value << "\n";
                    cout << "val_data_count = " << val_data_count << endl;
                #endif
            } catch (const exception& e) {
                cerr << "stoi(match[1].str()) Err: " << e.what() << "\n";
            }
        }
    }

    return results;
}

//==========================================================================


vector<int> parseBinarySocketData() {

    const uint16_t HEADER_SIZE = 4;

    vector<byte> socket_data_buf;
    
    {
        lock_guard<mutex> lock(data_mutex);  // Защита чтения

        // Проверяем, что буфер не пустой
        if (socket_data.empty()) {
            #ifdef DEBUG_COUT
                // cout << "socket_data is empty, returning empty results.\n";
            #endif
            return {};  // Возвращаем пустой вектор
        }

        // Перемещаем данные из socket_data в socket_data_buf
        socket_data_buf = move(socket_data);

        // Очищаем оригинальный буфер
        socket_data.clear();
    }

    vector<int> results;
    size_t pos = 0;

    while (pos < socket_data_buf.size()) {
        // Читаем заголовок пакета
        BinPacketHeader header;

        if (socket_data_buf.size() < HEADER_SIZE) {
            break;  // Недостаточно данных для чтения type и full_packet_size
        }
        header.type.low = (uint8_t)socket_data_buf[pos];
        pos++;    
        header.type.hi  = (uint8_t)socket_data_buf[pos];    
        pos++;    
        header.type.val = header.type.hi << 8 | header.type.low;    
        //===============
        header.full_packet_size.low = (uint8_t)socket_data_buf[pos];
        pos++;    
        header.full_packet_size.hi  = (uint8_t)socket_data_buf[pos];
        pos++;            
        header.full_packet_size.val = (header.full_packet_size.hi << 8) | header.full_packet_size.low;
       

        // Проверяем, совпадает ли тип пакета с целевым
        if (header.type.val == BYNARY_PACKET_KEY) {
            // Проверяем, хватает ли данных в векторе для чтения тела пакета
            if (pos + (header.full_packet_size.val - HEADER_SIZE) <= socket_data_buf.size()) {
                // Читаем тело пакета и добавляем его в выходной вектор
                for (size_t i = 0; i < (header.full_packet_size.val - 4); i += 2) {
                    int16_t value = (static_cast<int16_t>(socket_data_buf[pos + i + 1]) << 8) | static_cast<int16_t>(socket_data_buf[pos + i]);
                    results.push_back(value);
                    val_data_count++;
                }
                pos += (header.full_packet_size.val) - HEADER_SIZE;
            } else {
                // Недостаточно данных для чтения полного пакета
                break;
            }
        } else {
            // Пропускаем пакет с неподходящим типом
            pos += (header.full_packet_size.val);
        }
    }
    return results;
}
//================================================


void UpdateSocketPacketsPerSecond() {
    static int last_count = 0;
    int current_count = packet_count.load();
    int packets_this_second = current_count - last_count;
    last_count = current_count;

    packets_per_second.store(packets_this_second);
}

//================================================

void UpdateSocketValDataPerSecond() {
    static int last_count = 0;
    int current_count = val_data_count.load();
    int val_data_this_second = current_count - last_count;
    last_count = current_count;

    val_data_per_second.store(val_data_this_second);
}
//================================================

int Socket_Stop_Read() {
    //we need to close the socket descriptors after we're all done

    cout << "Socket_Stop_Read..." << endl;

    keep_running = false;

    return 0;   
}
//================================================

int Socket_Close() {
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    Stop_Socket_Thread(); 
    close(newSd);
    close(serverSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec) 
        << " secs" << endl;
    cout << "Connection closed..." << endl;

    var.socket.init_socket_done = 0;
    keep_running = false;

    return 0;   
}
//================================================

double GetPacketsPerSecond_S() {
    
    return packets_per_second;
}
//================================================

double Get_Val_Data_PerSecond_S() {
    
    return val_data_per_second;
}

//================================================

int interval_ms = 2000;
// Переменная для хранения времени последней проверки флага
auto last_check_time = chrono::steady_clock::now();

void Check_Socket_Connect() {

    if (need_to_reopen_socket_port) {
        need_to_reopen_socket_port = 0;
        
        // Close    
        if (var.socket.init_socket_done) {
            Socket_Close();
        }

        // Open
        //Clear_Socket_Data();
        Socket_Server_Init(var.socket.port);

        //var.socket.have_to_be_open = 1;
    }
    //=========================================================

    if (var.socket.have_to_be_binded) {

        // Ждём заданный промежуток времени с момента последней проверки
        auto current_time = chrono::steady_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(current_time - last_check_time).count();
        if (elapsed_time >= interval_ms) {
            
            cout << "Try_to_bind_Socket" << endl;
            // Обновляем время последней проверки
            last_check_time = current_time;

            //Socket_Server_Init(var.socket.port);
            int bind_status =  bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
            cout << "bind_status = " << bind_status << endl;
            if (bind_status == 0) {

                var.socket.have_to_be_binded = 0;


                cout << "Waiting for a client to connect..." << endl;
                //listen for up to 5 requests at a time
                listen(serverSd, 5);  
            
                //============================================
                state_wait_client = 1;

                if (wait_client_thread.joinable()) {
                    wait_client_thread.join();
                }
                
                // Запуск потока ожидания подключения клиента         
                wait_client_thread = thread(Wait_Socket_Client);

                var.socket.init_socket_done = 1;
                //============================================         
                        
            }
        }
    }    
}