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

using namespace std;

static std::atomic<int> is_binding(0);
static std::atomic<bool> need_to_reopen_socket_port(0);
static std::atomic<bool> state_wait_client(0);
static std::atomic<bool> keep_running(0);
static std::atomic<int> packet_count(0);
static std::atomic<int> packets_per_second(0);

static std::atomic<int> val_data_count(0);
static std::atomic<int> val_data_per_second(0);

static std::thread binding_thread;
static std::thread socket_thread;
static std::thread wait_client_thread;
static std::mutex data_mutex;

static std::vector<std::string> socket_data;

static char msg[1600];
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
        // wait_client_thread = std::thread(Wait_Socket_Client);

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

        std::cout << "Принято новое соединение" << std::endl;
        std::cout << "newSd=" << newSd << std::endl;   

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
    socket_thread = std::thread(ReadSocketData);

    // Запуск потока для обновления количества пакетов в секунду
    std::thread([]() {
        while (keep_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            UpdateSocketPacketsPerSecond();
            UpdateSocketValDataPerSecond();
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
    auto start_time = std::chrono::steady_clock::now();

    fd_set readfds;
    struct timeval timeout;

    while(keep_running)   {

        FD_ZERO(&readfds);
        FD_SET(newSd, &readfds);

        // Установка таймаута в 5 секунд
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;


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
            std::cout << "Таймаут: клиент неактивен" << std::endl;
            close(newSd);
            state_wait_client = 1;  // Сброс состояния ожидания клиента
            keep_running = 0;
            need_to_reopen_socket_port = 1;
            break;
        } else {
            if (FD_ISSET(newSd, &readfds)) {
                // Чтение данных от клиента

                //receive a message from the client (listen)
                //cout << "Awaiting client response..." << endl;
                memset(&msg, 0, sizeof(msg));//clear the buffer
                //bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);

                bytesRead_n = recv(newSd, (char*)&msg, sizeof(msg), 0);

                //cout << "recv =" << bytesRead<< endl;
                
                bytesRead +=bytesRead_n; 
                
                if(!strcmp(msg, "exit"))
                {
                    cout << "Client has quit the session" << endl;
                    break;
                }


                //cout << "Client: " << msg << endl;

                #ifdef DEBUG_COUT
                    cout <<  msg << endl;
                #endif
                //cout << ">";
                //string data;
                //getline(cin, data);
                //memset(&msg, 0, sizeof(msg)); //clear the buffer
                //strcpy(msg, data.c_str());
                // if(data == "exit")
                // {
                //     //send to the client that server has closed the connection
                //     send(newSd, (char*)&msg, strlen(msg), 0);
                //     break;
                // }

                if (var.socket.send.need_to_be_sended.load() == 1) {
                    var.socket.send.need_to_be_sended.store(0);
                    send(newSd, var.socket.send.message, strlen(var.socket.send.message), 0);
                    cout << "send to Client: " << var.socket.send.message << endl;
                }

                // Чтение данных
                int n = strlen(msg);
                if (n > 0) {
                    msg[n] = '\0';
                    std::string line(msg);
                    std::lock_guard<std::mutex> data_lock(data_mutex);

                    if (var.log.log_Is_Started) {
                        Add_Str_To_Log_File((uint8_t *)&msg, 0);
                    }
                    socket_data.push_back(line);
                    packet_count++;
                    #ifdef DEBUG_COUT
                        std::cout << "packet_count____________________________________" << packet_count << std::endl;
                    #endif 
                    //send the message to client
                    //bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);

                } else if (n < 0) {
                    std::cerr << "Ошибка чтения socket" << std::endl;
                    break;
                }
            }

        }          
    }
}

//====================================================================

std::vector<int> parseSocketData(const std::string& prefix) {

    std::vector<int> results;
    std::regex pattern(prefix + R"(\s+(\d+)*)");

    std::vector<std::string> socket_data_buf;
    std::lock_guard<std::mutex> lock(data_mutex); // Защита чтения
    socket_data_buf = std::move(socket_data);
 
    for (const auto& data : socket_data_buf) {
        #ifdef DEBUG_COUT
            std::cout << "data = " << data << "\n";
        #endif 
        //==================
        auto begin = std::sregex_iterator(data.begin(), data.end(), pattern);
        auto end = std::sregex_iterator();
    
        // Поиск всех вхождений префекса данных
        for(auto i = begin; i != end; ++i)  {
            std::smatch match = *i;
            string prefix_str = match.str();
            #ifdef DEBUG_COUT
                std::cout << "prefix_str = " << prefix_str << std::endl;
            #endif 
            // Вытаскиваем значение из каждой подстроки с префиксом.
            std::smatch match_1_val;
            if (std::regex_search(prefix_str, match_1_val, pattern)) {
                #ifdef DEBUG_COUT
                    std::cout << "results =" << std::stoi(match_1_val[1].str()) << "\n";    
                #endif  
                try {    
                    results.push_back(std::stoi(match_1_val[1].str())); 
                }  
                catch(...) {
                    std::cout << "stoi(match_1_val[1].str() Err "  << "\n"; 
                }      
                val_data_count++; 
                #ifdef DEBUG_COUT     
                    std::cout << "val_data_count = " << val_data_count << std::endl;  
                #endif      
            }
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
auto last_check_time = std::chrono::steady_clock::now();

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
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_check_time).count();
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
                wait_client_thread = std::thread(Wait_Socket_Client);

                var.socket.init_socket_done = 1;
                //============================================         
                        
            }
        }
    }    
}