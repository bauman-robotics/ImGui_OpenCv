#pragma once

void ReadSerialData();
std::vector<int> parseComPortData();


// std::vector<int> parseComPortData(const std::vector<std::string>& serial_data);

int InitSerial(); 
void ClearSerialData(); 
void CloseSerial();