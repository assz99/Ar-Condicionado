//Receiver

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <SSD1306.h>

const char *ssid = "LaboratorioE117";
const char *password = "LAB@E117";
int BROKER_PORT = 1883;
String IP = "172.24.1.2";
char ip_conv[14];
const char* BROKER_MQTT;

#define BAND    915E6  //é possivel setar a frequencia de 433MHz(433E6), 868Mhz(868E6),915Mhz(915E6)


String mensagem;
String localAddress;

char info_topico[25]; 

const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_SS_PIN = 18;
const int LORA_RST_PIN = 15;
const int LORA_DI00_PIN = 26;

//Dados IP REDE
IPAddress local_IP(172, 24, 1, 52);
IPAddress gateway(172, 24, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

//Variável para controlar o display
const int DISPLAY_ADDRESS_PIN = 0x3c;
const int DISPLAY_SDA_PIN = 4;
const int DISPLAY_SCL_PIN = 15;
const int DISPLAY_RST_PIN = 16;
SSD1306 display(DISPLAY_ADDRESS_PIN, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);

//variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne  = 1;

int trava_site = 0;
String mes;

WiFiClient espClient;


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
  setupDisplay();
  Serial.begin(115200);
  
  if (!loraBegin())
  {             // tenta inicializar a Rede LoRa
    Serial.println("LoRa nao conseguiu inicializar..");
    while (true);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
   LoRa.onReceive(onReceive);

   Serial.println("LoRa inicializado com sucesso");  
      Serial.println("Aguardando conexão");

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA não conseguiu configurar.");
  }

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
  display.drawString(0, 32, "[RECEIVER]");
  display.display(); 
  
    
}

void onReceive(int packetSize) 
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
 
  Serial.println("Tamanho da mensagem: " + String(incomingLength));
  Serial.println("Mensagem: " + incoming);
  Serial.println();
  String endWeb = "http://"+IP+"/acoes.php?atualizar=";
  endWeb += incoming;
  mes = endWeb;
  trava_site = 1;
}



void loop(){
  if(trava_site == 1){
    site();
    trava_site = 0;
    resetar();
    }
}

  void site(){  
  http.begin(mes);
  int httpCode = http.GET();
  http.end();
  
  }


void resetar(){
  if (millis() > 7200000){
    ESP.restart();
    }
  
  }
 
// Funcao para receber mensagem 
