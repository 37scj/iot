// lib do DHT11
#include <WiFiClientSecure.h>
#include <DHT.h>
//Include da lib de Wifi do ESP8266
#include <ESP8266WiFi.h>
// Lib do telegram
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>


//PIN
#define DHTTYPE DHT11
#define DHTPIN D4

// Seu token do telegram
#define BOTtoken "BOT_TOKEN_TELEGRAM" //Add seu botTOKEN

//Telegram.org Certificate 
const uint8_t fingerprint[20] = { 0xBB, 0xDC, 0x45, 0x2A, 0x07, 0xE3, 0x4A, 0x71, 0x33, 0x40, 0x32, 0xDA, 0xBE, 0x81, 0xF7, 0x72, 0x6F, 0x4A, 0x2B, 0x6B };

//Definir o SSID da rede WiFi
const char* ssid = "WIFI_NAME";
//Definir a senha da rede WiFi
const char* password = "WIFI_PASSWORD";

//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "b51f752b-3667-4f78-8306-8ce8e689410d";
const char* server = "api.tago.io";
int temp = 0;

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

String id, text;//Váriaveis para armazenamento do ID e TEXTO gerado pelo Usuario
long readDelay = 10000;     // Tempo em ms do intervalo a ser executado
unsigned long tempo;

//Funçao para Conectar ao wifi e verificar à conexao.
void connect()
{
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
   
  //Loop até conectar no WiFi
  WiFi.begin(ssid, password);
  //Caso nao esteja conectado ao WiFi, Ira conectarse
  while (WiFi.status() != WL_CONNECTED) {
    //Inicia o WiFi
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

  // fingerprint of Telegram server
  client.setFingerprint(fingerprint);
  client.setInsecure();
}

// Metodo responsavel por receber a temperatura e umidade e realizar uma requisicao do tipo POST para a api do Tago.io
void envia_dados(){
   
   float temp = FtoC(dht.readTemperature());
   float umidade = dht.readHumidity();  
   Serial.println(temp);
   Serial.println(umidade);
   delay(readDelay);
  
  //Inicia um client TCP para o envio dos dados
  if (client.connect(server,80)){

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

  }else{
        Serial.println("...connection failed!");
  }
  client.stop();
}

//Funçao que faz a leitura do Telegram.
void readTel()
{
   Serial.println("Obtendo mensagens");
   int newmsg = bot.getUpdates(bot.last_message_received + 1);
   Serial.println(newmsg);
   for (int i = 0; i < newmsg; i++)//Caso haja X mensagens novas, fara este loop X Vezes.
   {
      id = bot.messages[i].chat_id;//Armazenara o ID do Usuario à Váriavel.
      text = bot.messages[i].text;//Armazenara o TEXTO do Usuario à Váriavel.
      Serial.println(text);

      String from_name = bot.messages[i].from_name;    
      if (from_name == "") from_name = "Convidado";
    
      if (text == "/start") {
        String welcome = from_name + ", bem vindo ao Bot da Casa Viebrantz.\n";
        welcome += "Para interagir com a casa, use um dos comandos a seguir.\n\n";
        welcome += "/status : para saber o status dos sensores \n";
        bot.sendMessage(id, welcome, "Markdown");
      }
    
      if( text == "/status")//Caso o texto recebido contenha "ON"
      {
        float humidity = dht.readHumidity();
        float temperature = FtoC(dht.readTemperature());
        String message = "A temperatura é de " + String(temperature, 2) + " graus celsius.\n";
        message += "A umidade relativa do ar é de " + String(humidity, 2)+ "%.\n\n";      
        bot.sendMessage(id, message, "Markdown"); //Envia uma Mensagem para a pessoa que enviou o Comando.
      }else//Caso o texto recebido nao for nenhum dos acima, Envia uma mensagem de erro.
      {
         bot.sendSimpleMessage(id, "Comando Invalido", "");
      }
   }
}

// Metodo responsavel por fazer o calculo da temperura de farenheight para celsius
float FtoC(float fahr){ 
  return (fahr-32.0) * (5.0/9.0); 
}

void setup() {
  //Configuração da Porta serial
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);//Define o WiFi como Estaçao
  connect();//Funçao para Conectar ao WiFi
}

void loop() {

   if (millis() - tempo > 1000)//Faz a verificaçao das funçoes a cada 1 Segundos
   {
      connect();//Funçao para verificar se ainda há conexao
      readTel();//Funçao para ler o telegram
      envia_dados(); // //Envia dados para o tago.io
      tempo = millis();//Reseta o tempo
   }
        
}
