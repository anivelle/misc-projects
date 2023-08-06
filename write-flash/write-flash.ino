#include "mbed.h"
#include <cstdlib>

int *testWrite = reinterpret_cast<int *>(0x123);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
    };
    NRF_NVMC->CONFIG = 1;

    while (!NRF_NVMC->READY) {
    };
    // The first time the device gets turned on, this should write to flash and
    // then just spin lock. Defaults to 0xFFFFFFFF because the NVM is
    // initialized to all 1s
    if (*testWrite == 0xFFFFFFFF) {
        *testWrite = 13;
        while (true) {
        };
    }
    NRF_NVMC->CONFIG = 0;
}

void loop() {
    // The conditional is probably unnecessary but I felt like using it
    if (!*testWrite) {
        Serial.println(*testWrite);
    }
}
