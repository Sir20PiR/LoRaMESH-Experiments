#include <Arduino.h>
#include "LoraMesher.h"

#define BOARD_LED   4
#define LED_ON      LOW
#define LED_OFF     HIGH

LoraMesher& radio = LoraMesher::getInstance();

bool toggleCommand = LOW; // Alternância entre 0 e 1 a cada iteração

// Estrutura para o comando
struct LedCommand {
    uint8_t ledNumber;  // Número do LED (pode ser 1, 2, 3, etc.)
    uint8_t command = 1;    // Comando (por exemplo, 1 para ligar, 0 para desligar)
};

LedCommand* ledCommand = new LedCommand;

/**
 * @brief Inicializa a comunicação LoRa
 */
void setupLoraMesher() {
    radio.begin();
    radio.start();
    Serial.println("LoRa initialized");
}

/**
 * @brief Configurações iniciais do Arduino
 */
void setup() {
    Serial.begin(115200);
    pinMode(BOARD_LED, OUTPUT);
    setupLoraMesher();
}

/**
 * @brief Loop principal para enviar comandos alternados para acender e apagar o LED
 */
void loop() {
    // Prepara o comando para acender ou apagar o LED1
    ledCommand->ledNumber = 1;
    ledCommand->command = toggleCommand; // Alterna entre 0 e 1
    toggleCommand = !toggleCommand;

    // Envia o comando
    radio.createPacketAndSend(BROADCAST_ADDR, ledCommand, sizeof(LedCommand));
    
    // Aguarda antes de enviar o próximo comando
    delay(5000);
}
