#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <SSD1306.h>

const char *ssid = "Ar_Cond_IFMT";
const char *password = "ar_cond@123";
int BROKER_PORT = 1883;
String IP = "172.24.1.1";
char ip_conv[14];
const char* BROKER_MQTT;

const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_SS_PIN = 18;
const int LORA_RST_PIN = 15;
const int LORA_DI00_PIN = 26;

const int DISPLAY_ADDRESS_PIN = 0x3c;
const int DISPLAY_SDA_PIN = 4;
const int DISPLAY_SCL_PIN = 15;
const int DISPLAY_RST_PIN = 16;

#define BAND    915E6  //é possivel setar a frequencia de 433MHz(433E6), 868Mhz(868E6),915Mhz(915E6)
#define PABOOST true

String mensagem;
String localAddress;

char info_topico[25]; 
int estado = 0;

//Variável para controlar o display
SSD1306 display(DISPLAY_ADDRESS_PIN, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);

//variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne  = 1;

int potencia;
int Irms;
double kwhTotal_Acc;
double vlreais_Acc;

WiFiClient espClient;
PubSubClient MQTT(espClient); // instancia o mqtt

// Funcão para se conectar ao Broker MQTT
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}
HTTPClient http;

void setupDisplay(){
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


void setup() {
  IP.toCharArray(ip_conv, 14);
  BROKER_MQTT = ip_conv;
  setupDisplay();
  initMQTT();
  Serial.begin(115200);
  Serial.println(BROKER_MQTT);
  LoRa.setPins(18,14,26);
  if (!loraBegin())
  {             // tenta inicializar a Rede LoRa
    Serial.println("LoRa nao conseguiu inicializar..");
    while (true);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
  
   Serial.println("LoRa inicializado com sucesso");  
      Serial.println("Aguardando conexão");

      // Tenta conexão com Wi-fi
      WiFi.begin(ssid, password);
      while ( WiFi.status() != WL_CONNECTED ) {
              delay(100);
              Serial.print(".");
      }
  Serial.print("\nWI-FI conectado com sucesso: ");
  
  ////////////////////////////////////////
  // Convertendo o MAC para MAC byte
  ////////////////////////////////////////
  String MAC_LOCAL = WiFi.macAddress();
  char ChMacLocal[18];
  String(MAC_LOCAL).toCharArray(ChMacLocal, 18);
  char * InfoMacLocal[6];
  InfoMacLocal[0] = strtok (ChMacLocal,":");
  InfoMacLocal[1] = strtok (NULL,":");
  InfoMacLocal[2] = strtok (NULL,":");
  InfoMacLocal[3] = strtok (NULL,":");
  InfoMacLocal[4] = strtok (NULL,":");
  InfoMacLocal[5] = strtok (NULL,":");
  String Mac_Local_Full;
  for(int s=0; s<6; s++){
    Mac_Local_Full += InfoMacLocal[s];
  }
  localAddress = Mac_Local_Full;
  
  display.clear();
  display.drawString(0, 0, ""+String(MAC_LOCAL));
  display.drawString(0, 16, "Novo: "+String(Mac_Local_Full));
  display.drawString(0, 32, "[PRINCIPAL]");
  display.display(); 
  ////////////////////////////////////////
  // Convertendo o MAC para MAC byte
  ////////////////////////////////////////
  
  delay(500); //tempo para a tarefa iniciar
   
  //cria uma tarefa que será executada na função coreTaskOne, com prioridade 2 e execução no núcleo 1
  //coreTaskOne: atualizar as informações do display
  xTaskCreatePinnedToCore(
                    Outra_Tarefa,   /* função que implementa a tarefa */
                    "Outra_Tarefa", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    1,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreZero);         /* Núcleo que executará a tarefa */

    delay(200); //tempo para a tarefa iniciar

   //cria uma tarefa que será executada na função coreTaskTwo, com prioridade 2 e execução no núcleo 0
   //coreTaskTwo: vigiar o botão para detectar quando pressioná-lo
   xTaskCreatePinnedToCore(
                    Recebe_Info,   /* função que implementa a tarefa */
                    "Recebe_Info", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    1,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreOne);         /* Núcleo que executará a tarefa */
    delay(200); //tempo para a tarefa iniciar
    MQTT.connect("ESP do Ar");
    MQTT.subscribe("ar_cond/comando");
}

void loop(){}

void Recebe_Info( void * pvParameters ){
     while(true){
        //Serial.println(xPortGetCoreID());
        if(estado == 0){
          parserPacket(LoRa.parsePacket());
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (!MQTT.connected()) {
          MQTT.connect("ESP do Ar");
          MQTT.subscribe("ar_cond/comando");
        }
        while ( WiFi.status() != WL_CONNECTED ) {
          ESP.restart();
        }
        MQTT.loop();
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void Outra_Tarefa( void * pvParameters ){
     while(true){
        //Serial.println(xPortGetCoreID());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
 
// Funcao para receber mensagem 
void parserPacket(int packetSize) 
{
  if (packetSize == 0) return;          // Se nenhuma mesnagem foi recebida, retorna nada
  byte incomingLength = LoRa.read();    // Tamanho da mensagem
  String incoming = "";
  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) 
  {   
    Serial.println("erro!: o tamanho da mensagem nao condiz com o conteudo!");
    return;                        
  }
 
  Serial.println("Tamanho da   mensagem: " + String(incomingLength));
  Serial.println("Mensagem: " + incoming);
  Serial.println();
  String endWeb = "http://"+IP+"/acoes.php?atualizar=";
  endWeb += incoming;
  http.begin(endWeb);
  int httpCode = http.GET();
  http.end();
}

//Função que recebe as mensagens publicadas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  estado = 1;
  delay(200);
  Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));
  Serial.println(message);
  
  if(String(topic).equals("ar_cond/comando")){
  
  Enviar_CMD(String(message));
  Serial.println("Enviou");
  }
  delay(300);
  estado = 0; 
  Serial.flush();
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Tentando se conectar ao Broker MQTT: " + String(BROKER_MQTT));
    if (MQTT.connect("ESP do Ar")) {
      Serial.println("Conectado");
      MQTT.subscribe("ar_cond/comando");
    } else {
      Serial.println("Falha ao Reconectar");
      delay(1000);
    }
  }
}

void Enviar_CMD(String cmd) 
{
  LoRa.beginPacket();                   // Inicia o pacote da mensagem
  LoRa.write(cmd.length());        // Tamanho da mensagem em bytes
  LoRa.print(cmd);                 // Vetor da mensagem 
  LoRa.endPacket();                     // Finaliza o pacote e envia
  LoRa.receive();
}
