#ifndef _GT911_TOUCH_H
#define _GT911_TOUCH_H
#include <stdio.h>
#include <stdint.h>

class gsl3680_touch
{
public:
    gsl3680_touch(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin = -1, int8_t int_pin = -1);

    void begin();
    bool getTouch(uint16_t *x, uint16_t *y);
    void set_rotation(uint8_t r);

private:
    int8_t _sda, _scl, _rst, _int;

    // ---- vigilancia del controlador (ver healthCheck) ----
    void healthCheck();

    uint32_t lastHealthMs = 0;
    uint32_t lastReportMs = 0;
    uint32_t candidateSignature = 0;
    uint32_t goodSignature = 0;
    bool haveSignature = false;
    uint8_t badChecks = 0;
    uint8_t recoveries = 0;

    uint32_t readCount = 0;
    uint32_t downCount = 0;

    uint32_t lastSig = 0;
    int lastSigErr = 0;

    void handleSerialCommands();
};

#endif

