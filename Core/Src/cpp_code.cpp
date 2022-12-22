#include <cpp_code.hpp>
#include <main.h>

#include <stdint.h>
#include <stdio.h>

extern "C" {
void _kill(int pid, int sig) { (void)pid, (void)sig; }
int _getpid(void) { return -1; }
}

void myLoop(void) {
  uint8_t MSG[128] = {'\0'};
  uint8_t X = 0;
  while (1) {
    sprintf((char *)MSG, "Hello Dudes! Tracing X = %d\r\n", X);
    HAL_UART_Transmit(&huart3, MSG, sizeof(MSG), 100);
    HAL_Delay(500);
    X++;
  }
}
