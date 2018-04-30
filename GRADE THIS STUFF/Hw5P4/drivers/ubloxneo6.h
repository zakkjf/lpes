#include <stdint.h>

uint8_t detect_gps();

uint8_t init_gps(uint32_t module, uint32_t sysclock);

uint8_t get_gps(uint32_t module, char *msg);
