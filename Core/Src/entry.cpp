#include <default_task_logic.hpp>
#include <entry.hpp>

#include <cmsis_os2.h>
#include <main.h>

// TODO this will be generated
static void UserTask(void *argument) { DefaultTaskLogic(); }

static void BlinkTask(void *argument) {
  for (;;) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    osDelay(1000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    osDelay(1000);
  }
}

void entry(void) {
  osThreadAttr_t const userTask_attributes = {
      .name = "UserTask",
      .stack_size = 128 * 4, // TODO adjust
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(UserTask, NULL, &userTask_attributes);

  osThreadAttr_t const blinkTask_attributes = {
      .name = "BlinkTask",
      .stack_size = 128 * 4, // TODO adjust
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadNew(BlinkTask, NULL, &blinkTask_attributes);
}
