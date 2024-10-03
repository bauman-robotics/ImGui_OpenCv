#pragma once

int Socket_Server_Init(int port);
int Socket_Close();
int Socket_Stop_Read();
vector<float> parseSocketData_Float(const string& prefix);
vector<float> parseBinarySocketData_Float();
double GetPacketsPerSecond_S();
double Get_Val_Data_PerSecond_S();

void Check_Socket_Connect();