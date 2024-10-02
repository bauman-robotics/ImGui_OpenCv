#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>

#include "variables.h"

using namespace std;

static thread cpu_load_thread;
//=======================================================================

// Функция для получения данных из файла /proc/<pid>/stat
vector<string> getProcStat(pid_t pid) {
    ifstream stat_file("/proc/" + to_string(pid) + "/stat");
    if (!stat_file.is_open()) {
        throw runtime_error("Не удалось открыть файл /proc/<pid>/stat");
    }

    string line;
    getline(stat_file, line);
    vector<string> parts;
    istringstream iss(line);

    for (string part; getline(iss, part, ' '); ) {
        if (part.empty() || part == " ") continue;
        parts.push_back(part);
    }

    return parts;
}
//=======================================================================

// Функция для получения времени работы процессора
double getCPUTime(pid_t pid) {
    vector<string> stat = getProcStat(pid);
    double utime = stod(stat[13]);  // Пользовательское время
    double stime = stod(stat[14]);  // Системное время
    double cutime = stod(stat[15]); // Пользовательское время дочерних процессов
    double cstime = stod(stat[16]); // Системное время дочерних процессов

    return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
}
//=======================================================================

// Функция для получения загрузки процессора
double getCPULoad(pid_t pid, double interval) {
    struct timeval start, end;
    gettimeofday(&start, nullptr);

    double cpu_time_start = getCPUTime(pid);

    #ifdef DEBUG_COUT
           //cout << "getCPULoad()" << endl;
    #endif 

    this_thread::sleep_for(chrono::seconds(static_cast<int>(interval)));

    double cpu_time_end = getCPUTime(pid);
    gettimeofday(&end, nullptr);

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    if (elapsed_time <= 0) {
        throw runtime_error("Неверный интервал времени");
    }

    double cpu_usage = (cpu_time_end - cpu_time_start) / elapsed_time * 100.0;

    return cpu_usage;
}
//=======================================================================

// Функция, которая будет запущена в отдельном потоке
void measureCPULoad(pid_t pid, double interval, atomic<bool>& stop_flag) {

    // cout << "measureCPULoad()" << endl;
    // cout << "2_stop_flag.load() = " << stop_flag << endl;
    // cout << "interval = " << interval << endl;
    cout << "pid = " << pid << endl;    
    while (!stop_flag.load()) {
        try {
            var.cpu_load = getCPULoad(pid, interval);
            //cout << "Загрузка процессора: " << var.cpu_load << "%" << endl;
        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
        }
    }
}
//=======================================================================

void Get_CPU_Load_Init() {
    pid_t pid = getpid();
    double interval = 1.0; // Интервал в секундах
    static atomic<bool> stop_flag(false);

    //cout << "1_stop_flag() = " << stop_flag << endl;

    //cout << "Get_CPU_Load_Init()" << endl;
    // Создаем и запускаем поток для измерения загрузки процессора
    cpu_load_thread = thread(measureCPULoad, pid, interval, ref(stop_flag));


}
//=======================================================================

void Get_CPU_Load_DeInit() {
    // Устанавливаем флаг остановки
    //stop_flag.store(true);

    // Ждем завершения потока
    if (cpu_load_thread.joinable()) {
        cpu_load_thread.join();
    }

}
//=======================================================================
