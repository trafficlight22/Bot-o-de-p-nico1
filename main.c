#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUTTON_GPIO 4
#define BUZZER      GPIO_NUM_23
#define LED         5

#define ponto 200      
#define linha 900      

#define DEBOUNCE_TIME_MS 50
#define HOLD_TIME_MS     6000  // 6 segundos

int ativado = 0;


// Liga e desliga sirene-fernando
void tocarSirene() { gpio_set_level(BUZZER, 1); }
void pararSirene() { gpio_set_level(BUZZER, 0); }

// Mensagem de ativação-eloa
void enviarMensagem() { printf("SOS ATIVADO\n"); }

// Pisca o LED em Morse-Pedro
void morse(int tempo, int vezes, int entrada) {
    gpio_set_direction(entrada, GPIO_MODE_OUTPUT);
    for(int i = 0; i < vezes; i++) {
        gpio_set_level(entrada, 1);
        vTaskDelay(tempo / portTICK_PERIOD_MS);
        gpio_set_level(entrada, 0);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

// Letras do SOS
void s() {
    printf("S\n");
    morse(ponto, 3, LED);
    vTaskDelay(250 / portTICK_PERIOD_MS);
}

void o() {
    printf("O\n");
    morse(linha, 3, LED);
    vTaskDelay(250 / portTICK_PERIOD_MS);
}
void app_main(void) {

    printf("Sistema iniciado\n");

    //botão-eloa
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    // Configura LED-henrique e buzzer-fernando
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

    while (1) {
        //Verifica botão para ativação-eloa
        if (gpio_get_level(BUTTON_GPIO) == 0 && !ativado) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                int tempo = 0;
                //Espera segurar
                while (gpio_get_level(BUTTON_GPIO) == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                    tempo += 10;
                    if (tempo >= HOLD_TIME_MS) {
                        ativado = 1;
                        gpio_set_level(LED, 1);   // acende LED
                        tocarSirene();
                        enviarMensagem();         
                        break;
                    }
                }
            }
        }

        //  SOS em Morse enquanto ativado- Pedro 
        if (ativado) {
            s();
            o();
            s();
            vTaskDelay(500 / portTICK_PERIOD_MS); // pequeno intervalo

            // Permite desativar com botão
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
                if (gpio_get_level(BUTTON_GPIO) == 0) {
                    ativado = 0;
                    gpio_set_level(LED, 0);   // desliga LED
                    pararSirene();
                }
            }
        } else {
            vTaskDelay(10 / portTICK_PERIOD_MS); // loop leve quando inativo
        }
    }
}
