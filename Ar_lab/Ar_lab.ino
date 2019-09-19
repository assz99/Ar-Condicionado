#include <IRremote.h>
#include <SPI.h>
#include <LoRa.h>
#include "EmonLib.h"
#include <WiFi.h>
#include <Wire.h>
#include <SSD1306.h>
#include <SimpleDHT.h>

SimpleDHT22 dht22(23);

#define SSID "Ar-Condicionado-IFMT"
#define PASSWORD "123456789"
//int temp = 0;    // utilização dessa variavel somente para exemplo

const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_SS_PIN = 18;
const int LORA_RST_PIN = 15;
const int LORA_DI00_PIN = 26;

#define BAND 915E6 //é possivel setar a frequencia de 433MHz(433E6), 868Mhz(868E6),915Mhz(915E6)

double Irms;    // CORRENTE MEDIDA

EnergyMonitor SCT013;

int pinSCT = A0; //Pino analógico conectado ao SCT-013 36

int tensao = 220; // TENSÃO NOMINAL

int potencia; // POTENCIA CALCULADA

//Cria variaveis globais
double kwhTotal=0;
double kwhTotal_Acc=0;
double vlreais;
double vlreais_Acc;
double realPower = tensao * sqrt(2);
unsigned long ltmillis, tmillis, timems;
float humidade;
float temperatura;
int estado = 0;
unsigned long ultimoenvio = 1500;
String mensagem;
int c = 0;
String info;
int temp_ar = 20;

//Variável para controlar o display
const int DISPLAY_ADDRESS_PIN = 0x3c;
const int DISPLAY_SDA_PIN = 4;
const int DISPLAY_SCL_PIN = 15;
const int DISPLAY_RST_PIN = 16;
SSD1306 display(DISPLAY_ADDRESS_PIN, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);

WiFiServer sv(8080); //Cria o objeto servidor na porta 555
WiFiClient cl;       //Cria o objeto cliente.

String localAddress;
byte destination = 0xFF; // Endereco do dispositivo para enviar a mensagem (0xFF envia para todos devices)
long lastSendTime = 0;   // TimeStamp da ultima mensagem enviada
int interval = 1;
int intervall; // Intervalo em ms no envio das mensagens (inicial 5s)
int intervald;

//variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

//////////////////////////
IRsend irsend(13);
int enviar_cmd = 0;
int khz = 38; // 38kHz carrier frequency for the NEC protocol;
///////////////////////////
unsigned int liga[] = {3050,1600, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500};
unsigned int desliga[] = {3050,1600, 500,1050, 500,1050, 500,350, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,1050, 500,1050, 500,350, 500,1050, 500,300, 500,300, 500,1100, 500,1050, 500,300, 500,1100, 500,1050, 500,300, 500,300, 500,1100, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,1050, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,1050, 500,1100, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,1100, 500,300, 500,300, 500,300, 500,350, 450,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,1050, 500,350, 500,1050, 500,1050, 500,1100, 500,1050, 500,1050, 500,1050, 500,1100, 500};
unsigned int ar17[] = {3050,1550, 500,1100, 500,1050, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,1050, 550,300, 500,300, 500,1050, 500,1050, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,1050, 500,1100, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,350, 500,1050, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 500};  
unsigned int ar18[] = {3050,1550, 550,1000, 550,1050, 500,300, 500,300, 550,250, 550,1050, 500,300, 500,300, 550,1000, 550,1050, 500,300, 500,1050, 550,250, 550,300, 500,1050, 500,1050, 550,250, 550,1050, 500,1050, 500,300, 550,250, 550,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 550};  
unsigned int ar19[] = {3050,1550, 500,1050, 550,1050, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,1050, 500,300, 500,300, 500,1100, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,1100, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,1050, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,1100, 500,1050, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 450,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,1050, 500,300, 500,350, 500,300, 500,1050, 500,300, 500,350, 500,300, 500,300, 500};  
unsigned int ar20[] = {3050,1600, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,250, 550,1050, 500,1050, 500,300, 550,1000, 550,300, 500,300, 500,1050, 550,1000, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,1050, 550,1000, 550,300, 500,1050, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,1050, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550};  
unsigned int ar21[] = {3050,1550, 500,1050, 550,1050, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 550,250, 550,1050, 500,1050, 500,300, 550,250, 550,1050, 500,300, 500,300, 550,1000, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1000, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,1000, 550,300, 500,1050, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550};  
unsigned int ar22[] = {3050,1600, 500,1050, 500,1050, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,1050, 500,1050, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,1050, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 550,250, 550,300, 500,1050, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,1000, 550,1050, 500,300, 500,1050, 550,250, 550,300, 500,300, 500,300, 550,250, 550};  
unsigned int ar23[] = {3050,1600, 500,1050, 500,1050, 500,350, 500,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,350, 500,1050, 500,300, 500,300, 500,1100, 500,1050, 500,300, 500,1050, 500,1100, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,1050, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,1050, 500,300, 500,350, 500,1050, 500,1050, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,1050, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,1050, 500,300, 500,300, 500,1100, 500,300, 500,300, 500,300, 500,350, 500,300, 500};  
unsigned int ar24[] = {3050,1600, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500};  
unsigned int ar25[] = {3050,1550, 550,1050, 500,1050, 500,300, 550,250, 550,300, 500,1050, 500,300, 550,250, 550,1050, 500,1050, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,1050, 500,1050, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,1050, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 550,250, 550,1050, 500,300, 500,1050, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500}; 
unsigned int ar26[] = {3050,1600, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,1050, 500,300, 550,1050, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,1050, 500,1100, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500};  
unsigned int ar27[] ={3050,1550, 500,1050, 550,1050, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1000, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,300, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,250, 550,300, 500,1050, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500};  
unsigned int ar28[] = {3050,1550, 500,1100, 500,1050, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,1050, 500,300, 500,1050, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,1050, 500,1050, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,1050, 500,300, 500,300, 500,1050, 550,300, 500,300, 500,1050, 500,1050, 550,300, 500,300, 500,300, 500,300, 500,300, 550,300, 500,1050, 500,1050, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,1050, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 550,300, 500,300, 500,300, 500,300, 500,350, 500,300, 500,1050, 500,1050, 500,1100, 500,1050, 500,1050, 500,1050, 550,1050, 500,1050, 500,1050, 500}; 
///////////////////////////
//void Proto_Socket();
void setupDisplay()
{
  //O estado do GPIO16 é utilizado para controlar o display OLED
  pinMode(16, OUTPUT);
  //Reseta as configurações do display OLED
  digitalWrite(16, LOW);
  //Para o OLED permanecer ligado, o GPIO16 deve permanecer HIGH
  //Deve estar em HIGH antes de chamar o display.init() e fazer as demais configurações,
  //não inverta a ordem
  digitalWrite(16, HIGH);

  //Configurações do display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

bool loraBegin()
{
  // Iniciamos a comunicação SPI
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
  // Setamos os pinos do lora
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DI00_PIN);
  // Iniciamos o lora
  return LoRa.begin(BAND);
}

void setup()
{

  setupDisplay();
  //previousMillis = millis();
  delay(3000); //Aguarda 3 segundos
  SCT013.current(pinSCT, 5.4);
  Serial.begin(115200);

  LoRa.setPins(18, 14, 26);

  if (!loraBegin())
  { // tenta inicializar a Rede LoRa
    Serial.println("LoRa nao conseguiu inicializar..");
    while (true)
      ;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);

  sv.begin(); //Inicia o servidor TCP na porta declarada no começo.

  intervald = random(1800000, 1805501);
  intervall = random(20000, 35501);

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();

  Serial.println("LoRa inicializado com sucesso");

  ////////////////////////////////////////
  // Convertendo o MAC para MAC byte
  ////////////////////////////////////////
  String MAC_LOCAL = WiFi.macAddress();
  char ChMacLocal[18];
  String(MAC_LOCAL).toCharArray(ChMacLocal, 18);
  char *InfoMacLocal[6];
  InfoMacLocal[0] = strtok(ChMacLocal, ":");
  InfoMacLocal[1] = strtok(NULL, ":");
  InfoMacLocal[2] = strtok(NULL, ":");
  InfoMacLocal[3] = strtok(NULL, ":");
  InfoMacLocal[4] = strtok(NULL, ":");
  InfoMacLocal[5] = strtok(NULL, ":");
  String Mac_Local_Full;
  for (int s = 0; s < 6; s++)
  {
    Mac_Local_Full += InfoMacLocal[s];
  }
  localAddress = Mac_Local_Full;

  display.clear();
  display.drawString(0, 0, "" + String(MAC_LOCAL));
  display.drawString(0, 16, "Novo: " + String(Mac_Local_Full));
  display.drawString(0, 32, "[ESP - AR COND]");
  display.display();
  ////////////////////////////////////////
  // Convertendo o MAC para MAC byte
  ////////////////////////////////////////

  delay(500); //tempo para a tarefa iniciar

  //cria uma tarefa que será executada na função coreTaskOne, com prioridade 2 e execução no núcleo 1
  //coreTaskOne: atualizar as informações do display
  // xTaskCreatePinnedToCore(
  //  Proto_Socket,   /* função que implementa a tarefa */
  //  "Proto_Socket", /* nome da tarefa */
  //   10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
  //   NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
  // 1,          /* prioridade da tarefa (0 a N) */
  // NULL,       /* referência para a tarefa (pode ser NULL) */
  // taskCoreOne);         /* Núcleo que executará a tarefa */

  delay(500); //tempo para a tarefa iniciar

  //cria uma tarefa que será executada na função coreTaskTwo, com prioridade 2 e execução no núcleo 0
  //coreTaskTwo: vigiar o botão para detectar quando pressioná-lo
  xTaskCreatePinnedToCore(
    Comunicacao_Server,   /* função que implementa a tarefa */
    "Comunicacao_Server", /* nome da tarefa */
    10000,                /* número de palavras a serem alocadas para uso com a pilha da tarefa */
    NULL,                 /* parâmetro de entrada para a tarefa (pode ser NULL) */
    1,                    /* prioridade da tarefa (0 a N) */
    NULL,                 /* referência para a tarefa (pode ser NULL) */
    taskCoreZero);        /* Núcleo que executará a tarefa */
}

void temp_DHT()
{

  int err = SimpleDHTErrSuccess;

  if ((err = dht22.read2(&temperatura, &humidade, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
  }
  Serial.print(temperatura);
  Serial.print(" *C, ");
  Serial.print(humidade);
  Serial.println(" RH%");
}

void loop()
{
  Proto_Socket();
}

void enviar_info(int intervalo)
{
  if (millis() - lastSendTime > intervalo)
  {
    temp_DHT();
    info = localAddress + "?" + String(temperatura) + "?" + String(humidade) + "?" + String(Irms) + "?" + String(potencia) + "?" + String(kwhTotal_Acc, 5);

    Serial.println("Enviou as informações");

    sendMessage(info);
    lastSendTime = millis();
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
  parserPacket(LoRa.parsePacket());
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

void Comunicacao_Server(void *pvParameters)
{
  int counter = 1;
  while (true)
  {
    //Serial.println(xPortGetCoreID());
    // Calcula quantidade de tempo desde a última measurment realpower.
    ltmillis = tmillis;
    tmillis = millis();
    timems = tmillis - ltmillis;
    double Irms1 = SCT013.calcIrms(1480);
    double Irms2 = SCT013.calcIrms(1480);
    double Irms3 = SCT013.calcIrms(1480);
    Irms = (Irms1 + Irms2 + Irms3) / 3;
    // Calcula o valor da Corrente
    Serial.println("Irms = " + String(Irms, 2));
    if (Irms >= 0 and Irms <= 0.5)
    {
      Irms = 0;
      if (counter == 0) {
        interval = intervald;
      } else {
        enviar_info(0);
      }
      counter = 0;
    } else {
      interval = intervall;
      Irms = Irms;
      counter = 1;
    }

    potencia = Irms * tensao; // Calcula o valor da Potencia Instantanea
    // Calcular o número de hoje de kWh consumido.
    kwhTotal = kwhTotal + ((potencia / 1000.0) * 1.0 / 3600.0 * (timems / 1000.0));
    kwhTotal_Acc = kwhTotal_Acc + kwhTotal;
    //vlreais = kwhTotal * 0.35;
    //vlreais_Acc = vlreais_Acc + vlreais;
    
    enviar_info(interval);

  }
}

void Proto_Socket()
{
  while (true)
  {

    //Note the approach used to automatically calculate the size of the array.
    if (cl.connected()) //Detecta se há clientes conectados no servidor.
    {
      if (cl.available() > 0) //Verifica se o cliente conectado tem dados para serem lidos.
      {
        String req = "";
        while (cl.available() > 0) //Armazena cada Byte (letra/char) na String para formar a mensagem recebida.
        {
          char z = cl.read();
          req += z;
        }
        int res = req.toInt();

        comando_ar(res);
        //Envia uma resposta para o cliente
        cl.print("\nO servidor recebeu sua mensagem");
        cl.print("\n...Seu IP: ");
        cl.print(cl.remoteIP());
        cl.print("\n...IP do Servidor: ");
        cl.print(WiFi.softAPIP());
        cl.print("\n...Sua mensagem: " + req + "\n");
      }
    }
    else //Se nao houver cliente conectado,
    {
      cl = sv.available(); //Disponabiliza o servidor para o cliente se conectar.
      delay(1);
    }
  }
}

// Funcao que envia uma mensagem LoRa
void sendMessage(String mensagem)
{
  LoRa.beginPacket();            // Inicia o pacote da mensagem
  LoRa.write(mensagem.length()); // Tamanho da mensagem em bytes
  LoRa.print(mensagem);          // Vetor da mensagem
  LoRa.endPacket();              // Finaliza o pacote e envia
  Serial.println(mensagem);
}

// Funcao para receber mensagem
void parserPacket(int packetSize)
{
  if (packetSize == 0)
    return;                          // Se nenhuma mesnagem foi recebida, retorna nada
  byte incomingLength = LoRa.read(); // Tamanho da mensagem

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  {
    // check length for error
    Serial.println("erro!: o tamanho da mensagem nao condiz com o conteudo!");
    return;
  }
  char pay[100];
  String(incoming).toCharArray(pay, 100);
  char *InfoPay[2];
  InfoPay[0] = strtok(pay, "?");
  InfoPay[1] = strtok(NULL, "?");

  if (String(InfoPay[0]) != localAddress)
  {
    Serial.println("Esta mensagem não é para mim.");
    return;
  }
  /////////////////////////////////////////////
  int cmd_msg = String(InfoPay[1]).toInt();
  comando_ar(cmd_msg);
  ////////////////////////////////////////////////////////////
}
void comando_ar(int cmd)
{
  switch (cmd)
  {
    case (0):
      Serial.println("Desliga o Ar");
      irsend.sendRaw(desliga, sizeof(desliga) / sizeof(desliga[0]), khz);
      break;
    case (1):
      irsend.sendRaw(liga, sizeof(liga) / sizeof(liga[0]), khz);
      break;
    case (3):
      Serial.println("Diminui o Ar");
      temp_ar = temp_ar - 1;
      comando_ar(temp_ar);
      break;
    case (4):
      Serial.println("Aumenta o Ar");
      temp_ar = temp_ar + 1;
      comando_ar(temp_ar);
      break;
    case 17:
      Serial.println("Colocando o Ar em 17 C");
      irsend.sendRaw(ar17, sizeof(ar17) / sizeof(ar17[0]), khz);
      break;
    case 18:
      Serial.println("Colocando o Ar em 18 C");
      irsend.sendRaw(ar18, sizeof(ar18) / sizeof(ar18[0]), khz);
      break;
    case 19:
      Serial.println("Colocando o Ar em 19 C");
      irsend.sendRaw(ar19, sizeof(ar19) / sizeof(ar19[0]), khz);
      break;
    case 20:
      Serial.println("Colocando o Ar em 20 C");
      irsend.sendRaw(ar20, sizeof(ar20) / sizeof(ar20[0]), khz);
      break;
    case 21:
      Serial.println("Colocando o Ar em 21 C");
      irsend.sendRaw(ar21, sizeof(ar21) / sizeof(ar21[0]), khz);
      break;
    case 22:
      Serial.println("Colocando o Ar em 22 C");
      irsend.sendRaw(ar22, sizeof(ar22) / sizeof(ar22[0]), khz);
      break;
    case 23:
      Serial.println("Colocando o Ar em 23 C");
      irsend.sendRaw(ar23, sizeof(ar23) / sizeof(ar23[0]), khz);
      break;
    case 24:
      Serial.println("Colocando o Ar em 24 C");
      irsend.sendRaw(ar24, sizeof(ar24) / sizeof(ar24[0]), khz);
      break;
    case 25:
      Serial.println("Colocando o Ar em 25 C");
      irsend.sendRaw(ar25, sizeof(ar25) / sizeof(ar25[0]), khz);
      break;
    case 26:
      Serial.println("Colocando o Ar em 26 C");
      irsend.sendRaw(ar26, sizeof(ar26) / sizeof(ar26[0]), khz);
      break;
    case 27:
      Serial.println("Colocando o Ar em 27 C");
      irsend.sendRaw(ar27, sizeof(ar27) / sizeof(ar27[0]), khz);
      break;
    case 28:
      Serial.println("Colocando o Ar em 28 C");
      irsend.sendRaw(ar28, sizeof(ar28) / sizeof(ar28[0]), khz);
      break;
    default:
      Serial.println("Comando desconhecido");
      break;
  }
}
