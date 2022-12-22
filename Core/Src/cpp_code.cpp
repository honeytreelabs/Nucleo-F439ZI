#include <cpp_code.hpp>
#include <main.h>

#include <cstdint>
#include <cstdio>

#include <sstream>

void myLoop(void) {
  std::uint8_t X = 0;
  while (1) {
    std::stringstream msg;
    msg << "Hello Dudes! Tracing X = " << std::to_string(X) << "\r\n";
    HAL_UART_Transmit(&huart3,
                      reinterpret_cast<std::uint8_t const *>(msg.str().c_str()),
                      msg.str().length(), 100);
    HAL_Delay(500);
    X++;
  }
}
