#include "TM1638.h"
#include <cstdint>


TM1638::TM1638(PinName stb, PinName clk, PinName dio) : _stb(stb), _clk(clk), _dio(dio) {
    _dio.output();
}

void TM1638::init() {
    sendCommand(0x8F); // Enciende el display con brillo máximo
}

void TM1638::setBrightness(uint8_t brightness) {
    if (brightness > 7) brightness = 7;
    sendCommand(0x88 | brightness);
}

void TM1638::displayDigit(uint8_t position, uint8_t data) {
    sendData(position << 1, data);
}

void TM1638::displayNumber(uint32_t number) {
    for (int i = 0; i < 8; i++) {
        digit[i] = number % 10;
        displayDigit(i, digitToSegment[digit[7-i]]);
        number /= 10;
    }
}

void TM1638::clearDisplay() {
    for (int i = 0; i < 8; i++) {
        displayDigit(i, 0x00);
    }
}

void TM1638::clearLeds()
{
   
}
uint32_t TM1638::readKeys() {
    uint32_t keys = 0;

    _stb = 0;           // selecciona chip
    sendByte(0x42);     // comando "read key scan data"
    
    _dio.input();       // DIO -> entrada
    _dio.mode(PullUp);  // activar pull-up interno por si hace falta (opcional pero recomendable)

    for (int i = 0; i < 4; i++) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; j++) {
            _clk = 1;               // generar flanco (data válida mientras CLK está alto)
            wait_us(1);             // pequeña latencia
            int bit = _dio.read() & 1;
            byte |= (uint8_t)(bit << j); // LSB first dentro del byte
            _clk = 0;
            wait_us(1);
        }
        keys |= ((uint32_t)byte) << (i * 8); // colocar el byte en el sitio correcto
    }

    _dio.output();      // devolver DIO a salida
    _stb = 1;           // deseleccionar chip
    return keys;
}

void TM1638::setLed(uint8_t pos, bool state) {

   sendData((pos << 1)-1, state);

}

void TM1638::sendCommand(uint8_t command) {
    _stb = 0;
    sendByte(command);
    _stb = 1;
}

void TM1638::sendData(uint8_t address, uint8_t data) {
    sendCommand(0x44);
    _stb = 0;
    sendByte(0xC0 | address);
    sendByte(data);
    _stb = 1;
}

void TM1638::sendByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        _dio = (data & 0x01);
        _clk = 1;
        wait_us(1);
        _clk = 0;
        data >>= 1;
    }
}
