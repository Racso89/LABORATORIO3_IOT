#include "ESP32_MailClient.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <SoftwareSerial.h>
SoftwareSerial conexion;
SMTPData datosSMTP;
Servo servomotor;
int x;
String estado;//PUERTA
int pos = 0;
int servoPin = 4;
int rele= 5;
# define SENSOR  2
DHT dht(SENSOR,DHT11);
int T;
int H;
String GeneralPub;
String temperatura;
String humedad;
//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "ioticos.org";
const int mqtt_port = 1883;
const char *mqtt_user = "JbijC8Kc7JIoC4b";
const char *mqtt_pass = "8uTw7r1B26X2LHo";
const char *root_topic_subscribe = "VrAeLdmvqtf9RRe/temperatura";
const char *root_topic_subscribe1 = "VrAeLdmvqtf9RRe/puerta";
const char *root_topic_publish = "VrAeLdmvqtf9RRe/outgen";
const char *root_topic_publish_temp = "VrAeLdmvqtf9RRe/temp";
const char *root_topic_publish_hum = "VrAeLdmvqtf9RRe/hum";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "Hogar";
const char* password =  "195EDF8FD198D";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[100];
char msg_temp[25];
char msg_hum[25];


//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() 
{
    estado="Cerrado";
  servomotor.write(0);
  dht.begin();
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servomotor.setPeriodHertz(50);    // standard 50 hz servo
  servomotor.attach(servoPin, 500, 2400);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, 0);
  Serial.begin(9600);
  conexion.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() 
{
  
  if (!client.connected()) 
  {
    reconnect();
  }

  if (client.connected())
  {
    
  T= dht.readTemperature();
  H= dht.readHumidity();
  if(T>x && x>15)
  {
    correoVentilador();
    digitalWrite(rele, 1);

  }
  
  if(T<x && x>15)
  {
    digitalWrite(rele, 0); 
      
  }
 
    delay(2000);  
    GeneralPub ="Temperatura: "+ String(T)+"*****"+"Humedad: " + String(H)+"*****"+"puerta: "+ String(estado);
    GeneralPub.toCharArray(msg,100);
    client.publish(root_topic_publish,msg);

    temperatura ="Temperatura: "+ String(T);
    temperatura.toCharArray(msg_temp,25);
    client.publish(root_topic_publish_temp,msg_temp);
  
    humedad ="Humedad: "+ String(H);
    humedad.toCharArray(msg_hum,25);
    client.publish(root_topic_publish_hum,msg_hum);
  
  }
  client.loop();
  
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() 
{

  while (!client.connected()) 
  {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) 
    {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe))
      {
        Serial.println("Suscripcion ok");
      }
      else
      {
        Serial.println("fallo Suscripciión");
      }
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe1))
      {
        Serial.println("Suscripcion2 ok");
      }
      else
      {
        Serial.println("fallo Suscripciión2");
      }
    } else 
    {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}





//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length)
{
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) 
  {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);
  
  x = incoming.toInt();
 
   if(incoming=="Cerrar")
   {
     estado="Cerrado";
     servomotor.write(0);
     delay(2000);
   }
  
  if(incoming=="Abrir")
   {
     correoAbrir();
     estado="Abierto";
     servomotor.write(90);
     delay(2000);
   }
   
    
}
   

//*****************************
//***       CORREO ABRIR PUERTA       ***
//*****************************
void correoAbrir()
{
//Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
datosSMTP.setLogin("smtp.gmail.com", 465, "lab3iot@gmail.com", "12345IOT");
// Establecer el nombre del remitente y el correo electrónico
datosSMTP.setSender("CENTROL DE CONTROL", "lab3iot@gmail.com");
// Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
datosSMTP.setPriority("High");
// Establecer el asunto
datosSMTP.setSubject("ATENCION");
// Establece el mensaje de correo electrónico en formato de texto (sin formato)
datosSMTP.setMessage("PUERTA ABIERTA", false);
// Agregar destinatarios, se puede agregar más de un destinatario
datosSMTP.addRecipient("oscarivan@unisangil.edu.co");
 //Comience a enviar correo electrónico.
if (!MailClient.sendMail(datosSMTP))
Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
//Borrar todos los datos del objeto datosSMTP para liberar memoria
datosSMTP.empty();
delay(5000);
}


//*****************************
//***       CORREO VENTILADOR       ***
//*****************************
void correoVentilador()
{
//Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
datosSMTP.setLogin("smtp.gmail.com", 465, "lab3iot@gmail.com", "12345IOT");
// Establecer el nombre del remitente y el correo electrónico
datosSMTP.setSender("CENTROL DE CONTROL", "lab3iot@gmail.com");
// Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
datosSMTP.setPriority("High");
// Establecer el asunto
datosSMTP.setSubject("MONITOREO");
// Establece el mensaje de correo electrónico en formato de texto (sin formato)
datosSMTP.setMessage("VENTILADOR ENCENDIDO", false);
// Agregar destinatarios, se puede agregar más de un destinatario
datosSMTP.addRecipient("oscarivan@unisangil.edu.co");
 //Comience a enviar correo electrónico.
if (!MailClient.sendMail(datosSMTP))
Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
//Borrar todos los datos del objeto datosSMTP para liberar memoria
datosSMTP.empty();
delay(5000);
}
