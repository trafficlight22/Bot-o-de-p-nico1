#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUTTON_GPIO 4
#define DEBOUNCE_LIMIT 500

void app_main(void)
{
    int counter = 0;
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    while (1)
    {
        if (gpio_get_level(BUTTON_GPIO) == 0)
        {
            counter++;
            printf("%d\n",counter);
            if (counter >= DEBOUNCE_LIMIT)
            {
                printf("Pressionamento validado\n");
                counter = 0;

                while (gpio_get_level(BUTTON_GPIO) == 0)
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        else
        {
            counter = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

}

