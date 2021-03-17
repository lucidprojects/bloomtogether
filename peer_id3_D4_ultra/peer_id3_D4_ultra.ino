/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

int trigPin = 12;            // HC-SR04 trigger pin
int echoPin = 13;            // HC-SR04 echo pin
float duration, myDistance;
int myTrigger;


// REPLACE WITH THE RECEIVER'S MAC Address
//uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0x68, 0x07, 0xD4};
uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0x67, 0x78, 0xCC};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    int distance;
    int trigger;
} struct_message;

//Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

   pinMode(trigPin, OUTPUT); // define trigger pin as output
}
 
void loop() {

  getReadings();
  
  // Set values to send
  myData.id = 3;
  myData.distance = myDistance;
  myData.trigger = myTrigger;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(500);
}


void getReadings(){
//  temperature = bme.readTemperature();
//  humidity = bme.readHumidity();
//  pressure = (bme.readPressure() / 100.0F);

  digitalWrite(echoPin, LOW);   // set the echo pin LOW
  digitalWrite(trigPin, LOW);   // set the trigger pin LOW
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // set the trigger pin HIGH for 10μs
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);  // measure the echo time (μs)
  myDistance = (duration/2.0)*0.0343;   // convert echo time to distance (cm)
  if(myDistance>400 || myDistance<2) {
    Serial.println("Out of range");
   // servoMotor.write(90);
  }
  else
  {
    Serial.print("Distance: ");
    Serial.print(myDistance, 1); Serial.println(" cm");
    //servoMotor.write(0);
  }

  if(myDistance < 20) {
    Serial.println("we're in range");
//    servoMotor.write(45);
//    data = 45;
    myTrigger = 1; 
  }

  if(myDistance > 50) {
    //Serial.println("we're in range");
//    servoMotor.write(0);
     myTrigger = 0;
    
  }





}
