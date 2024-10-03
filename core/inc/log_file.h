#pragma once

using namespace std;

//===========================

void Create_Log_File(void);
void Add_Str_To_Log_File_ASCII(const vector<byte>&);
void Add_Str_To_Log_File_HEX_Float(const vector<float>&);

bool Open_Folder(const string& folder_name);