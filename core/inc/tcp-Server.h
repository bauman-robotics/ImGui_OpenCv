#pragma once

int Socket_Server_Init(int port);
int Socket_Close();
int Socket_Stop_Read();
std::vector<int> parseSocketData(const std::string& prefix);
std::vector<int> parseBinarySocketData(); 
double GetPacketsPerSecond_S();
double Get_Val_Data_PerSecond_S();

void Check_Socket_Connect();