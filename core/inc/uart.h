#pragma once

void ReadSerialData();
std::vector<int> parseComPortData();
int InitSerial(); 
void ClearSerialData(); 
void CloseSerial();
double GetPacketsPerSecond();