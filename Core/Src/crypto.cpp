#include <default_task_logic.hpp>
#include <main.h>

#include <stm32f4xx_hal_hash_ex.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

using RC = int;
constexpr static RC const RC_OK = 0;
constexpr static RC const RC_ERROR = -1;

static RC readUntil(UART_HandleTypeDef *huart, std::uint8_t *buf,
                    std::size_t buflen, std::uint8_t separator,
                    std::size_t *numRead) {
  for (std::size_t cnt = 0; cnt < buflen; ++cnt) {
    HAL_UART_Receive(huart, buf + cnt, 1, HAL_MAX_DELAY);
    if (buf[cnt] == separator) {
      buf[cnt] = '\0';
      *numRead = cnt;
      return RC_OK;
    }
  }
  *numRead = 0;
  return RC_ERROR;
}

static RC transmitMsg(UART_HandleTypeDef *huart, std::string const &msg) {
  HAL_UART_Transmit(huart, reinterpret_cast<std::uint8_t const *>(msg.c_str()),
                    msg.length(), HAL_MAX_DELAY);
  return RC_OK;
}

static RC dumpHex(UART_HandleTypeDef *huart, std::uint8_t *buf,
                  std::size_t len) {
  for (std::size_t cnt = 0; cnt < len; ++cnt) {
    std::array<char, 3> hexStr;
    snprintf(hexStr.data(), hexStr.size(), "%02x", buf[cnt]);
    transmitMsg(huart, hexStr.data());
  }
  return RC_OK;
}

void DefaultTaskLogic() {
  for (;;) {
    constexpr const std::size_t DIGEST_SIZE_SHA256 = 32; // bytes
    std::array<std::uint8_t, DIGEST_SIZE_SHA256> digest;
    std::array<std::uint8_t, 32> buf;

    std::size_t numRead;
    if (readUntil(&huart3, buf.data(), buf.size(), '\n', &numRead) != RC_OK) {
      transmitMsg(&huart3, "Could not read requested input.");
      continue;
    }
    HAL_HASHEx_SHA256_Start(&hhash, buf.data(), numRead /* not counting \0 */,
                            digest.data(), HAL_MAX_DELAY);
    dumpHex(&huart3, digest.data(), digest.size());
    transmitMsg(&huart3, "\r\n");
  }
}
