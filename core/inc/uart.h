#pragma once

void ReadSerialData();
std::vector<int> parseComPortData(const std::string& prefix);
int InitSerial(); 
void ClearSerialData(); 
void CloseSerial();
double GetPacketsPerSecond();