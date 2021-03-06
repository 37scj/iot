# Internet das coisas

Objetivo:

  Solução para coleta e envio de dados  via sensor DHT11 , Higrômetro com ESP8266.

IMAGEM                     |  IMAGEM  
:-------------------------:|:-------------------------:
 ![Fluxo](/assets/img-02.jpeg) |  ![Fluxo](/assets/imgReal.png) 

# Exemplo de esquematico:

IMAGEM                     |  IMAGEM  
:-------------------------:|:-------------------------:
![Fluxo](/assets/esquematico.PNG)  |  ![Fluxo](/assets/Capturar.PNG)

# Como funciona:

A cada 10 segundos o ESP8266 vai ler os dados do sensor e enviá-los 
através da conexão Wi-Fi a um servidor WEB via requisição HTTP(POST) Para
plataforma de monitoramento em nuvem Tago.io e disponibilizamos via chatbot utilizando o
canal de comunicação "Telegram" para solicitar o status de temperuta e umidade por texto.

![Fluxo](/assets/telegrama.png)
 
Usando Telegram :
IMAGEM                     |  IMAGEM  
:-------------------------:|:-------------------------:
![Fluxo](/assets/botfiap.jpeg)  |  ![Fluxo](/assets/img-01.jpeg)

  
# Usando Tago.io :
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
  - [Assista Video - 1](https://www.youtube.com/watch?v=Rnye98CtzP4 " 1 - Montagem")
  - [Assista Video - 2](https://www.youtube.com/watch?v=sgqao4sSsZ4 " 2 - Montagem")
  - [Assista Video - 3](https://www.youtube.com/watch?v=Sl8khDUYwcM " 3 - Montagem")


