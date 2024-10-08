#pragma once

void ReadSerialData();
std::vector<int> parseComPortData(const std::string& prefix);
int InitSerial(); 
void CloseSerial();
double GetPacketsPerSecond();
double Get_Val_Data_PerSecond();
std::vector<const char*> getConnectedTTYACMPorts();