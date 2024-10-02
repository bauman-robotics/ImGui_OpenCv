#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "main.h"
#include "log_file.h"
    
using namespace std;
namespace fs =filesystem;

void Create_Log_File(void);
void Add_Str_To_Log_File(uint8_t* strLog, int msg_len);

//===================================================================================================

void Create_Log_File(void) {
   string strFileAddName = "LogData";
   string str = "";

   string strDateTime = "";
   string strFolderName = "";
   string strFileName = "";
   string strFullFileLogName = "";

    // Получаем текущее локальное время
    auto now =chrono::system_clock::now();
   time_t now_time =chrono::system_clock::to_time_t(now);
   tm local_tm = *std::localtime(&now_time);

    // Формат времени: ГГГГ_ММ_ДД
   ostringstream oss;
    oss <<put_time(&local_tm, "%Y_%m_%d");
    strDateTime = oss.str();

    // Создаем папку с именем по текущей дате
    strFolderName = "./" + strDateTime;

    // Формат времени для имени файла: ЧЧ_ММ_СС
    oss.str("");  // Очищаем строковый поток
    oss.clear();  // Сбрасываем флаги состояния
    oss <<put_time(&local_tm, "%H_%M_%S");
    strFileName = oss.str() + "_" + strFileAddName;

    // Полное имя файла с расширением .csv
    strFullFileLogName = strFolderName + "/" + strFileName + ".csv";

    // Проверяем, существует ли директория, если нет — создаем
    if (!fs::exists(strFolderName)) {
        fs::create_directory(strFolderName);
    }

    // Создаем файл
    ofstream file(strFullFileLogName);
    if (file.is_open()) {
        // Запись данных в файл (если нужно)
        // file << str;
        file.close();

        var.log.curr_Log_File_Name = strFullFileLogName;
        var.log.log_Is_Started = true;

    } else {
       cerr << "Ошибка создания файла: " << strFullFileLogName <<endl;
    }
}
//===================================================================================================


// // Функция для добавления строки в лог-файл
// void Add_Str_To_Log_File(uint8_t* strLog, int msg_len) {
//     // Получаем полное имя текущего файла лога
//     string strFullName = var.log.curr_Log_File_Name;
    
//     // Создаем строку для записи
//     string str = "";

//     // Добавляем содержимое strLog к строке
//     str += reinterpret_cast<char*>(strLog);  // Преобразуем указатель на uint8_t* в строку

//     // Открываем файл для добавления строки в конец
//     ofstream file(strFullName, ios::app);  // Открываем файл в режиме добавления

//     if (file.is_open()) {
//         // Записываем строку в файл
//         file << str << endl;
//         file.close();
//     } else {
//         cerr << "Ошибка при открытии файла: " << strFullName << endl;
//     }
// }

// Функция для добавления строки в лог-файл
void Add_Str_To_Log_File(uint8_t* strLog, int msg_len) {
    // Получаем полное имя текущего файла лога
    string strFullName = var.log.curr_Log_File_Name;
    
    // Создаем строку для записи
    std::string str(reinterpret_cast<char*>(strLog), msg_len);

    // Открываем файл для добавления строки в конец
    ofstream file(strFullName, ios::app);  // Открываем файл в режиме добавления

    if (file.is_open()) {
        // Записываем строку в файл
        file << str << endl;
        file.close();
    } else {
        cerr << "Ошибка при открытии файла: " << strFullName << endl;
    }
}