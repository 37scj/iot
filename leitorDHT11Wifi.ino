// lib do DHT11
#include <DHT.h>
// Lib do telegram
#include <UniversalTelegramBot.h>
//Include da lib de Wifi do ESP8266
#include <ESP8266WiFi.h>

//PIN
#define DHTTYPE DHT11
#define DHTPIN D4

// Seu token do telegram
#define BOTtoken "1386192045:AAFMNQVjBkt-o--ip4KS1FLKn6oQeaThuis"  

//Intervalo para obter novas mensagens
#define BOT_SCAN_MESSAGE_INTERVAL 1000

// Ultima vez que buscou mensagem
long lastTimeScan; 

//Definir o SSID da rede WiFi
const char* ssid = "NOME_WIFI";
//Definir a senha da rede WiFi
const char* password = "SENHA_WIFI";

//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "b51f752b-3667-4f78-8306-8ce8e689410d";
const char* server = "api.tago.io";
int temp = 0;

UniversalTelegramBot bot(BOTtoken, client);
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

long previousMillis = 0;        // Variável de controle do tempo
long readDelay = 10000;     // Tempo em ms do intervalo a ser executado
unsigned long currentMillis = 0;

void setup() {
  //Configuração da Porta serial
  Serial.begin(9600);

  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  
  lastTimeScan = millis();
  
  //Inicia o WiFi
  WiFi.begin(ssid, password);

  //Loop até conectar no WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Logs na porta serial
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// Trata as novas mensagens que chegaram
void handleNewMessages(int numNewMessages) {
   for (int i=0; i<numNewMessages; i++) {
     
     String chat_id = String(bot.messages[i].chat_id);
     String text = bot.messages[i].text;
     
     // Pessoa que está enviando a mensagem
     String from_name = bot.messages[i].from_name;    
     if (from_name == "") from_name = "Convidado";
     
     if (text == "/start") {
        String welcome = from_name + ", bem vindo ao BOT FIAP.\n";
        welcome += "/env : saber a temperatura e umidade do ambiente \n";
        bot.sendMessage(chat_id, welcome, "Markdown");
    }
     
     if( text == "/env") {
      float humidity = dht.readHumidity();
      float temperature = FtoC(dht.readTemperature());
      String message = "A temperatura é de " + String(temperature, 2) + " graus celsius.\n";
      message += "A umidade relativa do ar é de " + String(humidity, 2)+ "%.\n\n";      
      bot.sendMessage(chat_id, message, "Markdown");
    }
     
  }
}

// Metodo responsavel por receber a temperatura e umidade e realizar uma requisicao do tipo POST para a api do Tago.io
void envia_dados(float temp,float umidade){

  //Inicia um client TCP para o envio dos dados
  if (client.connect(server,80)) {

    Serial.print("CONNECTED AT TAGO\n");

    String postStr = "";
    
    String postData = "[{\"variable\":\"temperature\",\"unit\":\"C\",\"value\":"+String(temp)+"},{\"variable\": \"umidade\",\"unit\": \"%\",\"value\": "+String(umidade)+"}]";

    postStr = "POST /data HTTP/1.1\n";
    postStr += "Host: api.tago.io\n";
    postStr += "Device-Token: "+apiKey+"\n";
    postStr += "_ssl: false\n";
    postStr += "Content-Type: application/json\n";
    postStr += "Content-Length: "+String(postData.length())+"\n";
    postStr += "\n";
    postStr += postData;

    client.print(postStr);
    Serial.println(postData);

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

  }
  client.stop();
}

// Metodo responsavel por fazer o calculo da temperura de farenheight para celsius
float FtoC(float fahr){ 
  return (fahr-32.0) * (5.0/9.0); 
}

//Enviando os dados de 10 em 10 segundos
void loop() {
  
  if (millis() > lastTimeScan + BOT_SCAN_MESSAGE_INTERVAL)  {
    // Serial.print("Checking messages - ");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  
    // Serial.println(numNewMessages);
    while(numNewMessages) {
      // Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeScan = millis();
  }
    
  delay(readDelay);

   float temp = FtoC(dht.readTemperature());
   float umidade = dht.readHumidity();   

   Serial.println(temp);
   Serial.println(umidade);
   
   //Envia dados para o tago.io
   envia_dados(temp,umidade);
  
}
