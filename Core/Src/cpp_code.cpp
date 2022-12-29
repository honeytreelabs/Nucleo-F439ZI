#include <cpp_code.hpp>
#include <main.h>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

using RC = int;
constexpr static RC const RC_OK = 0;
constexpr static RC const RC_ERROR = -1;

static RC readUntil(UART_HandleTypeDef *huart, std::uint8_t *buf,
                    std::size_t buflen, std::uint8_t separator) {
  for (std::size_t numRead = 0; numRead < buflen; ++numRead) {
    HAL_UART_Receive(huart, buf + numRead, 1, HAL_MAX_DELAY);
    if (buf[numRead] == separator) {
      buf[numRead] = '\0';
      return RC_OK;
    }
  }
  return RC_ERROR;
}

static RC transmitMsg(UART_HandleTypeDef *huart, std::string const &msg) {
  HAL_UART_Transmit(huart, reinterpret_cast<std::uint8_t const *>(msg.c_str()),
                    msg.length(), HAL_MAX_DELAY);
  return RC_OK;
}

void myLoop(void) {
  for (;;) {
    std::uint8_t buf[16] = {};

    try {
      // operand 1
      if (readUntil(&huart3, buf, sizeof(buf), ' ') != RC_OK) {
        transmitMsg(
            &huart3,
            "Operand 1: Could not read until separator into buffer.\r\n");
        continue;
      }
      float operand_1;
      operand_1 = std::stof(reinterpret_cast<char const *>(buf));

      // operator
      if (readUntil(&huart3, buf, sizeof(buf), ' ') != RC_OK) {
        transmitMsg(
            &huart3,
            "Operator: Could not read until separator into buffer.\r\n");
        continue;
      }
      auto const op = buf[0];

      // operand 2
      if (readUntil(&huart3, buf, sizeof(buf), '\n') != RC_OK) {
        transmitMsg(
            &huart3,
            "Operand 2: Could not read until separator into buffer.\r\n");
        continue;
      }
      auto const operand_2 = std::stof(reinterpret_cast<char const *>(buf));

      switch (op) {
      case '+':
        transmitMsg(&huart3, std::to_string(operand_1 + operand_2));
        break;
      case '-':
        transmitMsg(&huart3, std::to_string(operand_1 - operand_2));
        break;
      case '*':
        transmitMsg(&huart3, std::to_string(operand_1 * operand_2));
        break;
      case '/':
        transmitMsg(&huart3, std::to_string(operand_1 / operand_2));
        break;
      default:
        transmitMsg(&huart3, "Invalid operator given.");
        break;
      }
    } catch (std::invalid_argument const &exc) {
      transmitMsg(&huart3, "Could not parse number.");
    } catch (std::out_of_range const &exc) {
      transmitMsg(&huart3, "Number out of range.");
    }
    transmitMsg(&huart3, "\r\n");
  }
}
