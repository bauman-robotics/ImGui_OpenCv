#pragma once

void SaveCustomSettings(const char* filename);
void LoadCustomSettings(const char* filename);

void SaveWindowPosition(GLFWwindow* window, const char* ini_filename);
void LoadWindowPosition(GLFWwindow* window, const char* ini_filename);

void CleanIniFile(const char* ini_filename); 

void Set_Signal_Type_Flags(int signal_type); 