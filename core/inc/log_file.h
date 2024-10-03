#pragma once


using namespace std;

//===========================

void Create_Log_File(void);
void Add_Str_To_Log_File(uint8_t* strLog, int msg_len); 
void Add_Str_To_Log_File_HEX(vector<int>);

bool Open_Folder(const string& folder_name);
