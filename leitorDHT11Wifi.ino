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
#define RELE D3
#define LED_VERDE D2
#define LED_AZUL D1
#define SENSOR_AGUA A0

#define MAX_SUBSCRIBES 10

// Seu token do telegram
#define BOTtoken "BOT_TELEGRAM"

//Telegram.org Certificate
const uint8_t fingerprint[20] = { 0xBB, 0xDC, 0x45, 0x2A, 0x07, 0xE3, 0x4A, 0x71, 0x33, 0x40, 0x32, 0xDA, 0xBE, 0x81, 0xF7, 0x72, 0x6F, 0x4A, 0x2B, 0x6B };

//Definir o SSID da rede WiFi
const char* ssid = "NOME_WIFI";
//Definir a senha da rede WiFi
const char* password = "SENHA_WIFI";

//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "API_KEY_TAGO_IO";
const char* server = "api.tago.io";

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

float temp = 0;
float umidade = 0;
float agua_raw = 0;
float agua = 0;

// define o qual é o minimo de umidade na agua.
float limite_agua = 75;

String id, text;//Váriaveis para armazenamento do ID e TEXTO gerado pelo Usuario
long readDelay = 200;     // Tempo em ms do intervalo a ser executado

String subs[MAX_SUBSCRIBES];
String names[MAX_SUBSCRIBES];

//Funçao para Conectar ao wifi e verificar à conexao.
void connect()
{
  if (WiFi.status() != WL_CONNECTED) {

    Serial.print("Conectando na rede ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int qtd = 10;
    //Caso nao esteja conectado ao WiFi, Ira conectarse
    while (qtd > 0 && WiFi.status() != WL_CONNECTED) {
      //Inicia o WiFi
      delay(500);
      Serial.print(".");
      qtd--;
    }
    if (WiFi.status() == WL_CONNECTED) {
      //Logs na porta serial
      Serial.println("");
      Serial.println("WiFi conectado!");
      Serial.print("Rede ");
      Serial.println(ssid);
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

// Metodo responsavel por receber a temperatura e umidade e realizar uma requisicao do tipo POST para a api do Tago.io
void envia_dados() {
  if (apiKey == "API_KEY_TAGO_IO") return;

  readDHT(3);

  //Inicia um client TCP para o envio dos dados
  if (WiFi.status() == WL_CONNECTED && client.connect(server, 80)) {

    Serial.print("CONNECTED AT TAGO\n");

    String postData = "[";
    postData += "{\"variable\":\"water\",\"unit\":\"%\",\"value\":" + String(agua) + "},";
    postData += "{\"variable\":\"temperature\",\"unit\":\"C\",\"value\":" + String(temp) + "},";
    postData += "{\"variable\": \"humidity\",\"unit\": \"%\",\"value\": " + String(umidade) + "}";
    postData += "{\"variable\": \"water_raw\",\"unit\": \"\",\"value\": " + String(agua_raw) + "}";
    postData += "{\"variable\": \"water\",\"unit\": \"%\",\"value\": " + String(agua) + "}";
    postData += "]";

    String postStr = "";
    postStr = "POST /data HTTP/1.1\n";
    postStr += "Host: api.tago.io\n";
    postStr += "Device-Token: " + apiKey + "\n";
    postStr += "_ssl: false\n";
    postStr += "Content-Type: application/json\n";
    postStr += "Content-Length: " + String(postData.length()) + "\n";
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

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

  } else {
    Serial.println("...connection failed!");
  }
  client.stop();
}

//Funçao que faz a leitura do Telegram.
void readTel()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem conexão com a internet");
    return;
  }
  digitalWrite(LED_AZUL, HIGH);
  Serial.println("Obtendo mensagens do Telegram");
  int newmsg = bot.getUpdates(bot.last_message_received + 1);
  Serial.println("Novas mensagens: " + String(newmsg));
  for (int i = 0; i < newmsg; i++)//Caso haja X mensagens novas, fara este loop X Vezes.
  {
    id = bot.messages[i].chat_id;//Armazenara o ID do Usuario à Váriavel.
    text = bot.messages[i].text;//Armazenara o TEXTO do Usuario à Váriavel.

    String from_name = bot.messages[i].from_name;
    String message = "";

    Serial.println("Mensagem recebida de: " + from_name);
    Serial.println("Mensagem: " + text);

    if (text == "/start") {
      message = from_name + ", bem vindo ao BOT FIAP MBA 2021 , Criado por Lucas e Gabriel\n";
      message += "Para interagir com o bot, use um dos comandos a seguir.\n\n";
      message += "/status : para saber o status dos sensores \n";
    } else if (text == "/status") //Caso o texto recebido contenha "status"
    {
      readDHT(3);
      message = buildMessage();
    } else if (text.startsWith("/setagua")) //Caso o texto recebido contenha "set off"
    {
      message = "Configurando limite de agua de " + String(limite_agua) + " para ";
      limite_agua = text.substring(9).toFloat();
      message += String(limite_agua);
    } else if (text == "/seton") //Caso o texto recebido contenha "set on"
    {
      digitalWrite(RELE, HIGH);
      digitalWrite(LED_VERDE, LOW);
      message = "Rele " + readSwitch();
    } else if (text == "/setoff") //Caso o texto recebido contenha "set off"
    {
      digitalWrite(RELE, LOW);
      digitalWrite(LED_VERDE, HIGH);
      message = "Rele " + readSwitch();
    } else if (text == "/unsubscribe") //Caso o texto recebido contenha "set off"
    {
      for (int i = 0; i < MAX_SUBSCRIBES; i++) {
        if (subs[i] == id) {
          subs[i] = "";
          names[i] = "";
          message = "Cancelando inscrição com sucesso.";
        }
      }
    } else if (text == "/subscribe") //Caso o texto recebido contenha "set off"
    {
      message = "A fila está cheia. Tente novamente mais tarde.";
      int ok = 0;
      for (int i = 0; i < MAX_SUBSCRIBES; i++) {
        if ((ok == 0 && subs[i] == "") || (subs[i] == id)) {
          subs[i] = id;
          names[i] = from_name;
          message = from_name + ", voce foi inscrito na posição " + String(i);
          i = MAX_SUBSCRIBES;
          ok = 1;
        } else if (subs[i] == id) {
          subs[i] = "";
          names[i] = "";
        }
      }
    } else
    {
      //Caso o texto recebido nao for nenhum dos acima, Envia uma mensagem de erro.
      bot.sendSimpleMessage(id, "Comando Invalido", "");
    }
    if (message != "") {
      bot.sendMessage(id, message, "Markdown"); //Envia uma Mensagem para a pessoa que enviou o Comando.
      Serial.println("Mensagem de resposta: " + message);
    }
    enviarListaComandos(id);
  }
  enviarInscritos();
  digitalWrite(LED_AZUL, LOW);
}

void enviarListaComandos(String id) {
  bot.sendMessage(id, "/start /status /seton /setoff /setagua 80.0 \n/subscribe /unsubscribe", "Markdown");
  //Envia uma Mensagem para a pessoa que enviou o Comando.
}

String buildMessage() {
  String message = "A temperatura é de " + String(temp, 2) + " graus celsius.\n";
  message += "A umidade relativa do ar é de " + String(umidade, 2) + "%.\n";
  message += "A umidade da terra é de " + String(agua) + "%." + "(" + String(agua_raw) + ")" + "\n";
  message += "O relê está " + (readSwitch());
  return message;
}

void enviarInscritos() {
  int tem = 0;
  for (int i = 0; tem == 0 && i < MAX_SUBSCRIBES; i++) {
    if (subs[i] != "") tem = 1;
  }
  if (tem == 1) {
    readDHT(3);
    String message = buildMessage();
    message += "\n\nPara sair: /unsubscribe ";
    Serial.println(message);
    for (int i = 0; i < MAX_SUBSCRIBES; i++) {
      if (subs[i] != "") {
        Serial.println("Enviado leitura para " + String(i) + " id:" + subs[i] + " nome:" + names[i]);
        bot.sendMessage(subs[i], names[i] + ",\n" + message, "Markdown");
        enviarListaComandos(subs[i]);
      }
    }
  }

}
void readDHT(int tentativas) {
  Serial.println(".. Lendo DHT");
  temp = FtoC(dht.readTemperature());
  umidade = dht.readHumidity();
  Serial.print("Temperatura: ");
  Serial.println(temp);
  Serial.print("Umidade: ");
  Serial.println(umidade);

  agua_raw = analogRead(SENSOR_AGUA);
  Serial.print("Água: ");
  Serial.print(agua_raw);
  agua = map(agua_raw, 350, 1023, 100, 0);
  Serial.print(", mappedTo: ");
  Serial.println(agua);

  if (tentativas > 0 && (isnan(agua) || isnan(temp) || isnan(umidade))) {
    Serial.print(tentativas);
    delay(readDelay);
    readDHT(tentativas--);
  }

  if (!isnan(agua) && !isnan(limite_agua)) {
    digitalWrite(RELE, (agua <= limite_agua) ? HIGH : LOW);
  }
}

String readSwitch() {
  int r = digitalRead(RELE);
  return r == 0 ? "desligado (para ligar use /seton)" : "ativo (para desligar use /setoff)";
}

// Metodo responsavel por fazer o calculo da temperura de farenheight para celsius
float FtoC(float fahr) {
  return fahr;
  //return (fahr-32.0) * (5.0/9.0);
}

void setup() {
  // define rele
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, HIGH);
  // define o led verde
  pinMode(LED_VERDE, OUTPUT);
  digitalWrite(LED_VERDE, HIGH);
  // define o led verde
  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, HIGH);
  // define sensor de agua na porta analogica para leitura
  pinMode(SENSOR_AGUA, INPUT);

  // init inscricoes
  for (int i = 0; i < MAX_SUBSCRIBES; i++) subs[i] = "";

  //Configuração da Porta serial
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);//Define o WiFi como Estaçao
  connect();//Funçao para Conectar ao WiFi
  // fingerprint of Telegram server
  client.setFingerprint(fingerprint);
  client.setInsecure();


  // configuração padrão
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(RELE, LOW);
}

//Enviando os dados de 10 em 10 segundos
void loop() {

  connect();//Funçao para verificar se ainda há conexao
  readTel();//Funçao para ler o telegram
  envia_dados(); // //Envia dados para o tago.io
  readDHT(1);
  for (int i = 0; i < MAX_SUBSCRIBES; i++) {
    if (subs[i] != "") {
      Serial.println("Inscrito " + String(i) + ": " + subs[i] + ", ");
    }
  }
  delay(100);
}
