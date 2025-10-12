#ifndef MODBUSRTU_H
#define MODBUSRTU_H

#include <windows.h>
#include <vector>
#include <cstdint>
#include <string>

class ModbusRTU {
private:
    HANDLE hCom;
    bool connected;

public:
    ModbusRTU();
    ~ModbusRTU();

    // Основные методы работы с портом
    bool open(const char* portName, DWORD baudRate = CBR_9600);
    void close();
    bool isConnected() const;

    // Базовые операции ввода-вывода
    bool send(const std::vector<uint8_t>& data);
    bool receive(std::vector<uint8_t>& result, DWORD timeoutMs = 1000);
    bool query(const std::vector<uint8_t>& request, std::vector<uint8_t>& response, DWORD timeoutMs = 1000);

    // Статические методы для работы с протоколом Modbus
    static uint16_t calculateCRC(const std::vector<uint8_t>& data);

    // Создание Modbus запросов
    static std::vector<uint8_t> createReadRequest(uint8_t slaveId, uint16_t address, uint16_t count);
    static std::vector<uint8_t> createWriteSingleRegister(uint8_t slaveId, uint16_t address, uint16_t value);
    static std::vector<uint8_t> createWriteMultipleRegisters(uint8_t slaveId, uint16_t address, const std::vector<uint16_t>& values);
    static std::vector<uint8_t> createReadCoils(uint8_t slaveId, uint16_t address, uint16_t count);
    static std::vector<uint8_t> createWriteSingleCoil(uint8_t slaveId, uint16_t address, bool value);

    // Парсинг Modbus ответов
    static bool parseReadResponse(const std::vector<uint8_t>& response, std::vector<uint16_t>& registers);
    static bool parseWriteResponse(const std::vector<uint8_t>& response, uint16_t& address, uint16_t& value);
    static bool parseReadCoilsResponse(const std::vector<uint8_t>& response, std::vector<bool>& coils);

    // Вспомогательные методы
    static bool verifyCRC(const std::vector<uint8_t>& data);
    static std::string toHexString(const std::vector<uint8_t>& data);

private:
    // Внутренние вспомогательные методы
    void clearBuffers();
    bool setupPort(DWORD baudRate);
    bool setupTimeouts();
};

#endif // MODBUSRTU_H