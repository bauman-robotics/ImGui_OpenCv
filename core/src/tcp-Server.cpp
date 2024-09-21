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
// #include <boost/regex/escaped_string.hpp>

using namespace std;

static std::atomic<bool> keep_running(0);
static std::atomic<int> packet_count(0);
static std::atomic<int> packets_per_second(0);

static std::atomic<int> val_data_count(0);
static std::atomic<int> val_data_per_second(0);

static std::thread socket_thread;
static std::mutex data_mutex;

static std::vector<std::string> socket_data;

static char msg[1600];
static int newSd;
static int serverSd = 0;
static int bytesRead = 0;
static int bytesWritten = 0;
static struct timeval start1, end1;


int Socket_Server_Init(int port);
void Start_Server_Thread(); 
void Stop_Socket_Thread(); 
void ReadSocketData();
void UpdateSocketPacketsPerSecond();
void UpdateSocketValDataPerSecond();
int Socket_Close();
double GetPacketsPerSecond_S();
double Get_Val_Data_PerSecond_S();


//================================================

int Socket_Server_Init(int port) {
 
    if (!var.socket.init_socket_done) {
        //setup a socket and connection tools
        sockaddr_in servAddr;
        bzero((char*)&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);
    
        //open stream oriented socket with internet address
        //also keep track of the socket descriptor
        serverSd = socket(AF_INET, SOCK_STREAM, 0);

        if(serverSd < 0)
        {
            cerr << "Error establishing the server socket" << endl;
            exit(0);
        }
        //bind the socket to its local address
        int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
            sizeof(servAddr));
        if(bindStatus < 0)
        {
            cerr << "Error binding socket to local address" << endl;
            exit(0);
        }
        cout << "Waiting for a client to connect..." << endl;
        //listen for up to 5 requests at a time
        listen(serverSd, 5);
        //receive a request from client using accept
        //we need a new address to connect with the client
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        //accept, create a new socket descriptor to 
        //handle the new connection with client

        newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if(newSd < 0)
        {
            cerr << "Error accepting request from client!" << endl;
            exit(1);
        }
        cout << "Connected with client!" << endl;
        //lets keep track of the session time

        gettimeofday(&start1, NULL);
        //also keep track of the amount of data sent as well

        Start_Server_Thread(); 
        var.socket.init_socket_done = 1;
    } else {
        keep_running  = true;
    }

    return 0;   
}
//================================================

// Функция для запуска потока
void Start_Server_Thread() {
    keep_running  = true;

    // Запуск потока для чтения данных из COM-порта
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

    while(keep_running)
    {
        //receive a message from the client (listen)
        //cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
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

        // Чтение данных
        int n = strlen(msg);
        if (n > 0) {
            msg[n] = '\0';
            std::string line(msg);
            std::lock_guard<std::mutex> data_lock(data_mutex);
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