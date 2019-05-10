#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "# Cipriano";
const char* senha = "globo321";

const String token = "438C1C";

String float2str(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

bool conectaWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Conectando...");
    delay(250);
    return 0;
  } else
    return 1;
}

void wdt() {
  ESP.wdtFeed();
  yield();
}

void setup() {
  Serial.begin(9600);
  Serial.println("[ESP] Iniciando dispositivo...");

  //WiFi.persistent(false);
  //WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, senha);
  //WiFi.setSleep(false);
  
  conectaWiFi();
}

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

unsigned long tempoTotal = 0;
  
void loop() {
  if(conectaWiFi()){
    unsigned long tempoInicial = millis();

    //String dadosGet;
    String sinal = String(WiFi.RSSI());
    String dadosGet = "";

    if(tempoTotal > 0)
      dadosGet = "sinal=" + sinal + "&latencia=" + String(tempoTotal);
    else
      dadosGet = "sinal=" + sinal;

    
    String webservice = WiFiGET("http://sistema.rscada.ga/api/"+token+"/envio?"+dadosGet);

    tempoTotal = millis() - tempoInicial;
    
    while((millis() - tempoInicial) < 200) wdt();
  }
  wdt();
}