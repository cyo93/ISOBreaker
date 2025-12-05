#include "DataLogic.h"
bool compareBytes(size_t start, size_t end, uint8_t* input, uint8_t* magic) {
	for (size_t i = start; i < end; i++) {
		if (i + 1 == end && input[i] == magic[i - start])
		{
			return true;
		}
		else if (input[i] == magic[i - start]) {
			continue;
		}
		else {
			break;
		}
	}
	return false;
}

bool isBitSet(uint16_t number, uint16_t bit) {
	bit = 1 << bit;
	uint16_t result = number & bit;
	switch (result) {
	case 0:
		return false;
	default:
		return true;
	}
}
bool isEmpty(std::wstring string) {
	for (size_t i = 0; i < string.size(); i++) {
		if (string[i] != 0x20) {
			return false;
		}
	}
	return true;
}

std::string dateFormat(uint8_t* date) {
	uint32_t year = 1900 + date[0];
	uint8_t month = date[1];
	uint8_t day = date[2];
	uint8_t hour = date[3];
	uint8_t minute = date[4];
	uint8_t second = date[5];
	uint8_t offset = date[6];
	std::string output = std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(year);
	return output;
}

template <typename T>
std::wstring to_wstringPrecise(T value, uint32_t precision) {
	std::wostringstream os;
	os.precision(precision);
	os << std::fixed << value;
	return std::move(os).str();
}

std::wstring fileSizeConversion(uint32_t filesize) {
	if (filesize > 1073741824) {
		double size = double(filesize) / 1073741824;
		std::wstring ret = to_wstringPrecise(size, 2) + L" GB";
		return ret;
	}
	else if (filesize > 1048576) {
		double size = double(filesize) / 1048576;
		std::wstring ret = to_wstringPrecise(size, 2) + L" MB";
		return ret;
	}
	else if (filesize > 1024) {
		double size = double(filesize) / 1024;
		std::wstring ret = to_wstringPrecise(size, 2) + L" KB";
		return ret;
	}
	return std::to_wstring(filesize) + L" Bytes";
}

uint32_t bigInt32toLittleInt32(uint32_t input) {
	uint32_t output = ((input >> 24) & 0xff) | ((input << 8) & 0xff0000) | ((input >> 8) & 0xff00) | ((input << 8) & 0xff0000);
	return output;
}

uint16_t bigInt16toLittleInt16(uint16_t input) {
	uint16_t output = ((input >> 8) & 0xff) | ((input << 8));
	return output;
}

std::wstring lower(std::wstring a) {
	for (size_t i = 0; i < a.size(); i++) {
		a[i] = tolower(a[i]);
	}
	return a;
}

std::wstring nullTerminatedStr(std::wstring string) {
	std::wstring ret = L"";
	for (size_t i = 0; i < string.size(); i++) {
		if (string[i] == 0x00) {
			return ret;
		}
		ret += string[i];
	}
	return ret;
}

double round(double number, uint32_t places) {
	uint32_t power = std::pow(10, places);
	int round = number * power;
	number = round / power;
	return number;
}

std::wstring bigEndiantoLittleEndianUTF16(std::wstring string) {
	uint8_t* newstr = new uint8_t[string.size()];
	for (size_t i = 0; i < string.size(); i++) {
		newstr[i] = string[i];
	}
	std::wstring ret = L"";
	for (size_t i = 0; i < string.size(); i = i + 2) {
		uint16_t char16;
		memcpy_s(&char16, 2, newstr + i, 2);
		char16 = std::byteswap(char16);
		ret.push_back(char16);
	}
	delete[] newstr;
	return ret;
}
