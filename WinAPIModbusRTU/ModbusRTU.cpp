#include "ModbusRTU.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>
#include <sstream>
#include <iomanip>

ModbusRTU::ModbusRTU() : hCom(INVALID_HANDLE_VALUE), connected(false) {}

ModbusRTU::~ModbusRTU() {
    close();
}

bool ModbusRTU::open(const char* portName, DWORD baudRate) {
    std::string fullPortName = "\\\\.\\" + std::string(portName);

    hCom = CreateFileA(
        fullPortName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hCom == INVALID_HANDLE_VALUE) {
        return false;
    }

    if (!setupPort(baudRate) || !setupTimeouts()) {
        close();
        return false;
    }

    clearBuffers();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    connected = true;
    return true;
}

void ModbusRTU::close() {
    if (connected) {
        CloseHandle(hCom);
        connected = false;
    }
}

bool ModbusRTU::isConnected() const {
    return connected;
}

bool ModbusRTU::send(const std::vector<uint8_t>& data) {
    if (!connected) return false;

    DWORD bytesWritten;

    PurgeComm(hCom, PURGE_TXCLEAR);

    BOOL result = WriteFile(hCom, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, NULL);

    if (!result || bytesWritten != data.size()) {
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true;
}

bool ModbusRTU::receive(std::vector<uint8_t>& result, DWORD timeoutMs) {
    if (!connected) return false;

    result.clear();
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        DWORD bytesAvailable;
        COMSTAT status;
        if (ClearCommError(hCom, NULL, &status)) {
            bytesAvailable = status.cbInQue;
        }
        else {
            bytesAvailable = 0;
        }

        if (bytesAvailable > 0) {
            std::vector<uint8_t> buffer(bytesAvailable);
            DWORD bytesRead = 0;

            if (ReadFile(hCom, buffer.data(), bytesAvailable, &bytesRead, NULL) && bytesRead > 0) {
                result.insert(result.end(), buffer.begin(), buffer.begin() + bytesRead);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
        }

        if (!result.empty()) {
            break;
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

        if (elapsed.count() > timeoutMs) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return !result.empty();
}

bool ModbusRTU::query(const std::vector<uint8_t>& request, std::vector<uint8_t>& response, DWORD timeoutMs) {
    if (!send(request)) {
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return receive(response, timeoutMs);
}

uint16_t ModbusRTU::calculateCRC(const std::vector<uint8_t>& data) {
    uint16_t crc = 0xFFFF;

    for (uint8_t byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            }
            else {
                crc = crc >> 1;
            }
        }
    }

    return crc;
}

std::vector<uint8_t> ModbusRTU::createReadRequest(uint8_t slaveId, uint16_t address, uint16_t count) {
    std::vector<uint8_t> request = {
        slaveId,
        0x03,
        static_cast<uint8_t>((address >> 8) & 0xFF),
        static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>((count >> 8) & 0xFF),
        static_cast<uint8_t>(count & 0xFF)
    };

    uint16_t crc = calculateCRC(request);
    request.push_back(static_cast<uint8_t>(crc & 0xFF));
    request.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return request;
}

std::vector<uint8_t> ModbusRTU::createWriteSingleRegister(uint8_t slaveId, uint16_t address, uint16_t value) {
    std::vector<uint8_t> request = {
        slaveId,
        0x06,
        static_cast<uint8_t>((address >> 8) & 0xFF),
        static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>(value & 0xFF)
    };

    uint16_t crc = calculateCRC(request);
    request.push_back(static_cast<uint8_t>(crc & 0xFF));
    request.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return request;
}

std::vector<uint8_t> ModbusRTU::createWriteMultipleRegisters(uint8_t slaveId, uint16_t address, const std::vector<uint16_t>& values) {
    std::vector<uint8_t> request = {
        slaveId,
        0x10,
        static_cast<uint8_t>((address >> 8) & 0xFF),
        static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>((values.size() >> 8) & 0xFF),
        static_cast<uint8_t>(values.size() & 0xFF),
        static_cast<uint8_t>(values.size() * 2)
    };

    for (uint16_t value : values) {
        request.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        request.push_back(static_cast<uint8_t>(value & 0xFF));
    }

    uint16_t crc = calculateCRC(request);
    request.push_back(static_cast<uint8_t>(crc & 0xFF));
    request.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return request;
}

std::vector<uint8_t> ModbusRTU::createReadCoils(uint8_t slaveId, uint16_t address, uint16_t count) {
    std::vector<uint8_t> request = {
        slaveId,
        0x01,
        static_cast<uint8_t>((address >> 8) & 0xFF),
        static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>((count >> 8) & 0xFF),
        static_cast<uint8_t>(count & 0xFF)
    };

    uint16_t crc = calculateCRC(request);
    request.push_back(static_cast<uint8_t>(crc & 0xFF));
    request.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return request;
}

std::vector<uint8_t> ModbusRTU::createWriteSingleCoil(uint8_t slaveId, uint16_t address, bool value) {
    std::vector<uint8_t> request = {
        slaveId,
        0x05,
        static_cast<uint8_t>((address >> 8) & 0xFF),
        static_cast<uint8_t>(address & 0xFF),
        value ? static_cast<uint8_t>(0xFF) : static_cast<uint8_t>(0x00),
        static_cast<uint8_t>(0x00)
    };

    uint16_t crc = calculateCRC(request);
    request.push_back(static_cast<uint8_t>(crc & 0xFF));
    request.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return request;
}

bool ModbusRTU::parseReadResponse(const std::vector<uint8_t>& response, std::vector<uint16_t>& registers) {
    if (response.size() < 5) return false;

    if (response[1] != 0x03) return false;

    uint8_t byteCount = response[2];
    if (response.size() != 3 + byteCount + 2) return false;

    if (!verifyCRC(response)) return false;

    registers.clear();
    for (int i = 0; i < byteCount; i += 2) {
        uint16_t reg = (response[3 + i] << 8) | response[4 + i];
        registers.push_back(reg);
    }

    return true;
}

bool ModbusRTU::parseWriteResponse(const std::vector<uint8_t>& response, uint16_t& address, uint16_t& value) {
    if (response.size() != 8) return false;

    if (response[1] != 0x06) return false;

    if (!verifyCRC(response)) return false;

    address = (response[2] << 8) | response[3];
    value = (response[4] << 8) | response[5];

    return true;
}

bool ModbusRTU::parseReadCoilsResponse(const std::vector<uint8_t>& response, std::vector<bool>& coils) {
    if (response.size() < 4) return false;

    if (response[1] != 0x01) return false;

    uint8_t byteCount = response[2];
    if (response.size() != 3 + byteCount + 2) return false;

    if (!verifyCRC(response)) return false;

    coils.clear();
    for (int i = 0; i < byteCount; i++) {
        uint8_t byte = response[3 + i];
        for (int bit = 0; bit < 8; bit++) {
            coils.push_back((byte >> bit) & 0x01);
        }
    }

    return true;
}

bool ModbusRTU::verifyCRC(const std::vector<uint8_t>& data) {
    if (data.size() < 2) return false;

    std::vector<uint8_t> dataWithoutCRC(data.begin(), data.end() - 2);
    uint16_t calculatedCRC = calculateCRC(dataWithoutCRC);
    uint16_t receivedCRC = (data[data.size() - 1] << 8) | data[data.size() - 2];

    return calculatedCRC == receivedCRC;
}

std::string ModbusRTU::toHexString(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    for (size_t i = 0; i < data.size(); i++) {
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
        if (i < data.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}


void ModbusRTU::clearBuffers() {
    if (connected) {
        PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }
}

bool ModbusRTU::setupPort(DWORD baudRate) {
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(hCom, &dcb)) {
        return false;
    }

    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fInX = FALSE;
    dcb.fOutX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fAbortOnError = FALSE;

    return SetCommState(hCom, &dcb);
}

bool ModbusRTU::setupTimeouts() {
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    return SetCommTimeouts(hCom, &timeouts);
}