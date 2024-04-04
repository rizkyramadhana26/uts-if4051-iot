#include <WiFi.h>
#include <PubSubClient.h>

//define configurations
int previous_state;
int saldo = 251000;
int freq = 2;
long previousMillis = 0;
int ledState = LOW;
const char* ssid = "THE BARRAN LT 3A";
const char* password = "barranatas";
const char* mqtt_server = "192.168.100.45";
bool blinking = false;
bool menyala = false;
String transactions = "+ 251000;";

WiFiClient espClient;
PubSubClient client(espClient);

//reconnect MQTT connection
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    client.connect(clientId.c_str());
  }
  client.subscribe("ESP/INPUT");
  client.publish("ESP/OUTPUT",transactions.c_str());
  Serial.println("Connected!");
}

void setup() {
  //setup led and button
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  while(!Serial)
  previous_state = HIGH;

  //connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* message, unsigned int length){
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (String(topic) == "ESP/INPUT") {
    transactions += "+ ";
    transactions += messageTemp;
    transactions += ";";
    saldo += atoi(messageTemp.c_str());
    client.publish("ESP/OUTPUT",transactions.c_str());
  }
}

void loop() {
  //connect to MQTT Server
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //detect if button is clicked
  int current_state = digitalRead(0);
  if((previous_state==HIGH) && (current_state==LOW)){
    if(saldo>=20000){
      saldo -= 20000;
      transactions += "- 20000;";
      client.publish("ESP/OUTPUT", transactions.c_str());
      digitalWrite(2, HIGH);
      delay(5000);
      digitalWrite(2, LOW);

    }else{
      String msg = "Transaksi gagal! Saldo akhir: Rp";
      msg += saldo;
      client.publish("ESP/OUTPUT",transactions.c_str());
      for(int i=0;i<5;i++){
        digitalWrite(2, HIGH);
        delay(500);
        digitalWrite(2, LOW);
        delay(500);
      }
    }
  }else if((previous_state==LOW) && (current_state==HIGH)){
    previous_state = HIGH;
  }
}
