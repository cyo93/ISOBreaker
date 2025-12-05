#ifndef DATA_LOGIC_H
#define DATA_LOGIC_H
#include <bit>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
bool compareBytes(size_t start, size_t end, uint8_t* input, uint8_t* magic);
uint32_t bigInt32toLittleInt32(uint32_t input);
std::wstring fileSizeConversion(uint32_t filesize);

template <typename T>
std::wstring to_wstringPrecise(T string, uint32_t precision);

bool isBitSet(uint16_t number, uint16_t bit);
bool isEmpty(std::wstring string);
uint16_t bigInt16toLittleInt16(uint16_t input);
double round(double number, uint32_t places);
std::string dateFormat(uint8_t* date);
std::wstring lower(std::wstring a);
std::wstring bigEndiantoLittleEndianUTF16(std::wstring string);
std::wstring nullTerminatedStr(std::wstring string);
#endif