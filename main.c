#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definições
#define BUTTON_GPIO 4
#define LED_STATUS  2
#define BUZZER      GPIO_NUM_23

#define DEBOUNCE_TIME_MS 50
#define HOLD_TIME_MS     6000  // 6 segundos

int ativado = 0;

// Sirene
void tocarSirene() {
    gpio_set_level(BUZZER, 1);
}

void pararSirene() {
    gpio_set_level(BUZZER, 0);
}

void enviarMensagem() {
    printf("SOS ATIVADO\n");
}

void app_main(void)
{
    // Configura botão com pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    // LED e buzzer
    gpio_set_direction(LED_STATUS, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

    while (1)
    {
        // Detecta botão pressionado
        if (gpio_get_level(BUTTON_GPIO) == 0 && !ativado)
        {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));

            if (gpio_get_level(BUTTON_GPIO) == 0)
            {
                int tempo = 0;

                // ⏱️ Espera segurar botão
                while (gpio_get_level(BUTTON_GPIO) == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                    tempo += 10;

                    if (tempo >= HOLD_TIME_MS)
                    {
                        ativado = 1;

                        gpio_set_level(LED_STATUS, 1);
                        tocarSirene();
                        enviarMensagem();

                        break;
                    }
                }
            }
        }
        if (ativado)
        {
            if (gpio_get_level(BUTTON_GPIO) == 0)
            {
                vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));

                if (gpio_get_level(BUTTON_GPIO) == 0)
                {
                    ativado = 0;

                    gpio_set_level(LED_STATUS, 0);
                    pararSirene();
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
