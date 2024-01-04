#include <Arduino.h>
#include "LoraMesher.h"

#define LED   21  // Número do LED a ser controlado (LED21 no receptor)
#define LED_ON HIGH
#define LED_OFF LOW

LoraMesher& radio = LoraMesher::getInstance();

// Estrutura para representar um comando para o LED
struct LedCommand {
    uint8_t ledNumber;  // Número do LED
    uint8_t command;    // Comando (1 para ligar, 0 para desligar)
};

LedCommand* receivedCommand = new LedCommand;

/**
 * @brief Controla o estado do LED com base no comando recebido
 * 
 * @param ledNumber Número do LED
 * @param command   Comando para o LED (1 para ligar, 0 para desligar)
 */
void ledControl(uint8_t ledNumber, uint8_t command) {
    if (ledNumber == 1) {
        digitalWrite(LED, command == 1 ? LED_ON : LED_OFF);
    }
    // Adicione mais lógica conforme necessário para outros LEDs
}

/**
 * @brief Imprime o comando recebido para um LED específico
 * 
 * @param command Comando para o LED
 */
void printLedCommand(LedCommand command) {
    Serial.printf("LED%d Command: %s\n", command.ledNumber, command.command == 1 ? "ON" : "OFF");
    Serial.println();
}

/**
 * @brief Processa o pacote de dados recebido
 * 
 * @param packet Pacote de dados recebido
 */
void printDataPacket(AppPacket<LedCommand>* packet) {
    Serial.printf("Packet arrived from %X with size %d\n", packet->src, packet->payloadSize);

    LedCommand* cmd = packet->payload;
    size_t payloadLength = packet->getPayloadLength();

    for (size_t i = 0; i < payloadLength; i++) {
        printLedCommand(cmd[i]);
        ledControl(cmd[i].ledNumber, cmd[i].command);
    }
}

/**
 * @brief Tarefa para processar os pacotes recebidos
 * 
 * @param parameter Parâmetro da tarefa (não utilizado)
 */
void processReceivedPackets(void* parameter) {
    for (;;) {
        ulTaskNotifyTake(pdPASS, portMAX_DELAY);
        while (radio.getReceivedQueueSize() > 0) {
            AppPacket<LedCommand>* packet = radio.getNextAppPacket<LedCommand>();
            printDataPacket(packet);
            radio.deletePacket(packet);
        }
    }
}

TaskHandle_t receiveLoRaMessage_Handle = NULL;

/**
 * @brief Cria a tarefa para receber mensagens LoRa
 */
void createReceiveMessages() {
    int res = xTaskCreate(
        processReceivedPackets,
        "Receive App Task",
        4096,
        (void*)1,
        2,
        &receiveLoRaMessage_Handle);

    if (res != pdPASS) {
        Serial.printf("Error: Receive App Task creation gave error: %d\n", res);
    }

    radio.setReceiveAppDataTaskHandle(receiveLoRaMessage_Handle);
}

/**
 * @brief Inicializa a comunicação LoRa
 */
void setupLoraMesher() {
    radio.begin();
    createReceiveMessages();
    radio.start();
    Serial.println("LoRa initialized");
}

/**
 * @brief Configurações iniciais do Arduino
 */
void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    setupLoraMesher();
}

/**
 * @brief Loop principal (não utilizado neste contexto)
 */
void loop() {
    // Nada a ser feito no loop principal
}