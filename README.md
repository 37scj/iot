# Internet das coisas

Objetivo:

  Solução para coleta e envio de dados  via sensor DHT11 e ESP8266.

![Fluxo](/assets/imgReal.png)

# Exemplo de esquematico DHT11 + ESP8266:

![Fluxo](/assets/esquematico.PNG)

# Como funciona:

A cada 10 segundos o ESP8266 vai ler os dados do sensor e enviá-los 
através da conexão Wi-Fi a um servidor WEB via requisição HTTP(POST) Para
plataforma de monitoramento em nuvem Tago.io e disponibilizamos via chatbot utilizando o
canal de comunicação "Telegram" para solicitar o status de temperuta e umidade por texto.

Usando Telegram :
  ![Fluxo](/assets/telegrama.png)
  ![Fluxo](/assets/botfiap.jpeg)
  
Usando Tago.io :
  ![Fluxo](/assets/img.png)

# Sobre o codigo:

  Estamos utilizando tres bibliotecas para realizar a captura dos dados do DHT11 
  e conexão de wifi para envio de informações via procolo https com o esp2866 no NodeMCU e uma
  lib para sincronizar informações com o telegram.
    
   - No código foi definido em qual o pino o DHT11 se encontra, no caso é o DHTPIN D4, a 
     representação do DHTPIN D4 é a porta GPIO02.   
     
     - ![circuito](/assets/circuito.jpg)
     
   - Setamos o `SSID` da rede WiFi e senha.
   
   - Setamos o endpoint e API Key fornecida pelo tago.io.
   - Setamos o endpoint e API Key do Telegram
    
   - No método setup nos setamos a porta serial 9600 para facilitar a leitura de informação e 
     utilizamos os recursos da biblioteca esp2866 para realizar uma conexão via wifi. realizamos
     configurações para definir o rele , leds e conexão wifi.
     
   - O Metodo `loop` é responsável por verificar conexão do wifi, por ser
     um ouvinte para receber mensagens do telegram, disparo de informações para o tago.io e
     obter os dados de temperatura,umidade,agua com um delay de 0,1 segundos.
     
      
# Video:

  - [Video](https://www.youtube.com/watch?v=Rnye98CtzP4 "Montagem")


