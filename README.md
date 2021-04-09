# Internet das coisas

Objetivo:

  Solução para coletar dados via sensor DHT11 de temperatura e umidade.

![Fluxo](/nodemcu.png)

# Exemplo de esquematico DHT11 + ESP8266:


# Como funciona:

A cada 10 segundos o ESP8266 vai ler os dados do sensor e enviá-los 
através da conexão Wi-Fi a um servidor WEB via requisição HTTP(POST) Para
plataforma de monitoramento em nuvem Tago.io

![Fluxo](/img.png)

# Sobre o codigo:

  Estamos utilizando duas bibliotecas para realizar a captura dos dados do dht11 
  e conexão de wifi para envio de informações via procolo https com o esp2866.
  
  ![circuito](/circuito.jpg)
  
     - No Codigo nós definimos em qual o pino o DHT11 se encontra, no nosso caso é o DHTPIN D4, a 
      representação do DHTPIN D4 é a portal GPIO02.
      
     - Setamos o SSID da rede WiFi e senha
     - Endpoint e API Key fornecida pelo tago.io
     - No metodo setup nos setamos a porta seria 9600 para facilitar a leitura de informação e 
        utilizamos os recursos da biblioteca esp2866 para realizar uma conexão via wifi

      
# Video:

[Video](https://www.youtube.com/watch?v=Rnye98CtzP4 "Montagem")







  


