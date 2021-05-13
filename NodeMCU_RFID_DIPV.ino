//Bibliotecas do RFID
#include <SPI.h>
#include <MFRC522.h>
//Bibliotecas do NodeMCU
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define SS_PIN  4  //D2
#define RST_PIN 5  //D1

MFRC522 mfrc522(SS_PIN, RST_PIN); // Criação de uma instância MFRC522

AsyncWebServer server(8080); //declaração da porta 8080 para comunicação com o host

const char *ssid = "Jamoraes";
const char *password = "5668840760439331858";
const char* device_token  = "70156c4933d83f56"; //token do dispositivo gerado no site

String URL = "http://192.168.15.27:8080/rfidattendance/getdata.php"; // ipv4 da minha rede
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin(); //Inicia o módulo SPI
  mfrc522.PCD_Init(); // Inicia o módulo RC-522
  //---------------------------------------------
  connectToWiFi();
}

void loop() {
  //Procura pela minha rede wi-fi
  if(!WiFi.isConnected()){
    connectToWiFi();    //Conecta a rede wi-fi
  }
  
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);

  //Procura por um nova ID do cartão
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//Retorna caso não ache nenhum ID
  }
  // Seleciona o cartão
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  
//  Realiza o print da ID da tag
  SendCardID(CardID);
  delay(1000);
}
//Envia a ID do cartão para o site
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;    //Declara como objetvo HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Adiciona a ID do cartão pra pegar o GET array para enviar
    //GET methode
    Link = URL + getData;
    http.begin(Link); //Inicia o request do HTTP   
    
    int httpCode = http.GET();   //Envia o request
    String payload = http.getString();    

//    Serial.println(Link);   
    Serial.println(httpCode);   //Mostra código de retorno HTTP que é == 200
    Serial.println(Card_uid);     //Mostra a ID do cartão/tag
    Serial.println(payload);    //Mostra a resposta do request (sucesso

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
    

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
  
        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();  //Encerra conexão
    }
  }
}
//Void da conexão ao wi-fi
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Evita problema de reconexão 
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Mostra o endereço IP do Node
    
    delay(1000);
}
