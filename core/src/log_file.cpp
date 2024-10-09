#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "main.h"
#include "log_file.h"
#include "defines.h"

#include <cstdlib>
#include <unistd.h> // для access()
#include <sys/stat.h> // для stat()

#include <vector>
#include <iomanip>  // Для std::fixed и std::setprecision


using namespace std;
namespace fs =filesystem;

void Create_Log_File(void);

void Add_Str_To_Log_File_ASCII(const vector<byte>& vec_byte_packet); 
void Add_Str_To_Log_File_HEX_Float(const vector<float>& vec_float_packet);
string Vector_Float_To_String(const vector<float>& vector, int precision); 
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
   tm local_tm = *localtime(&now_time);

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
        var.log.currentFolderName = strFolderName;
        var.log.log_Is_Started = true;

    } else {
       cerr << "Ошибка создания файла: " << strFullFileLogName <<endl;
    }
}
//===================================================================================================

// Функция для добавления строки в лог-файл
void Add_Str_To_Log_File_ASCII(const vector<byte>& vec_byte_packet) {
    // Получаем полное имя текущего файла лога
    string strFullName = var.log.curr_Log_File_Name;

    // Преобразуем вектор байтов в строку (предполагая, что байты содержат ASCII символы)
    string str(reinterpret_cast<const char*>(vec_byte_packet.data()), vec_byte_packet.size());

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
//===================================================================================================

// Функция для добавления строки в лог-файл
void Add_Str_To_Log_File_HEX_Float(const vector<float>& vec_float_packet) {
  #ifndef LOG_ONE_VAL_TO_LINE
    string str;
    str = Vector_Float_To_String(vec_float_packet, FLOAT_PRECISION);
  #endif
  // Открываем файл для добавления строки в конец
  ofstream file(var.log.curr_Log_File_Name, ios::app);  // Открываем файл в режиме добавления

  if (file.is_open()) {
      #ifdef LOG_ONE_VAL_TO_LINE
      // Записываем каждое значение на новой строке
      for (float value : vec_float_packet) {
          file << value << endl;
      }
      #else 
        // Записываем строку в файл
        file << str << endl;
      #endif 
      file.close();
  } else {
      cerr << "Ошибка при открытии файла: " << var.log.curr_Log_File_Name << endl;
  }
}
//====================================================================

bool Open_Folder(const string& folder_name) {
  // Проверяем, существует ли папка и является ли она директорией.
  struct stat s;
  if (stat(folder_name.c_str(), &s) == 0) {
    if (S_ISDIR(s.st_mode)) {
      // Формируем команду для xdg-open.
      string command = "xdg-open \"" + folder_name + "\"";

      // Выполняем команду.
      int result = system(command.c_str());

      // Проверяем на наличие ошибок.  0 обычно означает успех.
      if (result == 0) {
        return true;
      } else {
        cerr << "Ошибка при выполнении xdg-open: " << result << endl;
        return false;
      }
    } else {
      cerr << "Ошибка: '" << folder_name << "' не является директорией." << endl;
      return false;
    }
  } else {
    cerr << "Ошибка: '" << folder_name << "' не существует." << endl;
    return false;
  }
}
//====================================================================



string Vector_Float_To_String(const vector<float>& vector, int precision) {
    stringstream ss;
    ss << fixed << setprecision(precision);  // Ограничиваем количество знаков после запятой
    
    // Проходим по каждому элементу вектора
    for (size_t i = 0; i < vector.size(); ++i) {
        ss << vector[i];  // Добавляем элемент в строковый поток
        if (i != vector.size() - 1) {
            ss << " ";  // Добавляем пробел, если это не последний элемент
        }
    }
    
    return ss.str();  // Возвращаем строку из строкового потока
}

