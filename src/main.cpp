/*

RScada v0.1a
Projeto com a finalidade de medição de latência e nível de sinal de um Wi-Fi.
Envio: MQTT
Programação: Arduino
Microcontrolador: ESP32

*/

#include <WiFi.h>
//#include <HTTPClient.h>

const char* ssid = "# Cipriano";
const char* senha = "globo321";

#include <MQTT.h>

WiFiClient net;
MQTTClient mqtt;

//const String token = "438C1C";
const char* token = "438C1C";

String float2str(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

bool conectaWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Conectando à "+String(ssid)+"...");
    delay(250);
    return 0;
  } else
    return 1;
}

void wdt() {
  yield();
}

void setup() {
  Serial.begin(9600);
  Serial.println("[ESP] Iniciando dispositivo...");

  //WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, senha);
  WiFi.setSleep(false);
  
  conectaWiFi();
    
  mqtt.begin("mqtt.rscada.ga", net);
  mqtt.connect(token, token, token);
}

/*
String WiFiGET(String link) {
  if (!conectaWiFi()) return "";

  HTTPClient http;
  http.begin(link);
  http.setTimeout(2000);

  int httpCode = http.GET(); // Recebe o código que o servidor retornou.

  if (httpCode == HTTP_CODE_OK) { // Se a conexão obtiver sucesso, executa o código abaixo.
    String resposta = http.getString(); // Recebe o conteúdo da página.
    http.end();
    //Serial.println(resposta);
    return resposta;
  }

  http.end(); // Encerra conexão HTTP.
  return "";
}
*/

unsigned long tempoTotal = 0;
  
void loop() {
  if(conectaWiFi()){
    //String dadosGet;
    String sinal = String(WiFi.RSSI());

    unsigned long tempoInicial = millis();    
    /*
    String dadosGet = "";

    if(tempoTotal > 0)
      dadosGet = "sinal=" + sinal + "&latencia=" + String(tempoTotal);
    else
      dadosGet = "sinal=" + sinal;
        
    String webservice = WiFiGET("http://sistema.rscada.ga/api/"+token+"/envio?"+dadosGet);
    */

    if(mqtt.connected()){
      mqtt.publish(String(token)+"/sinal", sinal, false, 1);
      tempoTotal = millis() - tempoInicial;

      if(tempoTotal > 0){
        mqtt.publish(String(token)+"/latencia", String(tempoTotal), false, 1);
        Serial.println("[MQTT] Informações enviadas ao servidor.\nLatência: "+String(tempoTotal)+" ms - Nível de Sinal: "+sinal+" dBm");
      } else
        Serial.println("[MQTT] Informações enviadas ao servidor.\nNível de Sinal: "+sinal+" dBm");
    } else {
      mqtt.disconnect();
      mqtt.connect(token, token, token);
    }

    while((millis() - tempoInicial) < 200) wdt();
  }
  wdt();
}