#pragma once

int Socket_Server_Init(int port);
int Socket_Close();
int Socket_Stop_Read();
std::vector<int> parseSocketData(const std::string& prefix);
double GetPacketsPerSecond_S();
double Get_Val_Data_PerSecond_S();