#pragma once

int OpenSerialPort(const char* device);
void ReadSerialData(int fd, std::vector<std::string>& serial_data);
int ExtractDataValue(const std::string& input);
void CleanSerialData(std::string& data);

std::vector<int> parseComPortData(const std::vector<std::string>& serial_data);