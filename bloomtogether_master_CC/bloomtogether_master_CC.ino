#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <esp_now.h>
#include <WiFi.h>


#include <ESP32Servo.h> //esp32

Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position

#define LED_PIN    13 //esp32

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 12

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const int trigPin = 26;  //grey
const int echoPin = 27; // purple
float duration, myDistance;
int myTrigger;

boolean bloomed = false;
int bloomTo;


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int distance;
  int trigger;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;

// Create an array with all the structures
struct_message boardsStruct[4] = {board1, board2, board3, board4};

int board1Trigger;
int board1Distance;
int board1Id;
int board2Trigger;
int board2Distance;
int board2Id;

int board3Trigger;
int board3Distance;
int board3Id;
int board4Trigger;
int board4Distance;
int board4Id;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id - 1].distance = myData.distance;
  boardsStruct[myData.id - 1].trigger = myData.trigger;
  boardsStruct[myData.id - 1].id = myData.id;
  Serial.printf("id value: %d \n", boardsStruct[myData.id - 1].id);
  Serial.printf("distance value: %d \n", boardsStruct[myData.id - 1].distance);
  Serial.printf("trigger value: %d \n", boardsStruct[myData.id - 1].trigger);
  Serial.println();
}




void setup() {

  //Initialize Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);


  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  myservo.attach(12); // esp32  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //  Serial.begin(9600);

}

void loop() {
  //  getReadings();
  checkTriggers();
}

void breathAll() {
  for (int i = 0; i < 187; i++) {
    for (int j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, strip.Color(i + 15, i, 0)); // yellow with a little extra red to make it warmer
    }
    strip.show();
    delay(30);
  }
  for (int i = 187; i > 0; i--) {
    for (int j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, strip.Color(i + 15, i, 0));
    }
    strip.show();
    delay(30);
  }
}

void checkTriggers() {
  // Acess the variables for each board
  board1Trigger = boardsStruct[0].trigger;
  board1Distance = boardsStruct[0].distance;
  board1Id = boardsStruct[0].id;
  board2Trigger = boardsStruct[1].trigger;
  board2Distance = boardsStruct[1].distance;
  board2Id = boardsStruct[1].id;

  board3Trigger = boardsStruct[2].trigger;
  board3Distance = boardsStruct[2].distance;
  board3Id = boardsStruct[2].id;
  board4Trigger = boardsStruct[3].trigger;
  board4Distance = boardsStruct[3].distance;
  board4Id = boardsStruct[3].id;


//  if ((board1Trigger == 1 && board1Distance < 20) && (board2Trigger == 1 && board2Distance < 20)) {
if ((board1Trigger == 1 && board1Distance < 20) && (board2Trigger == 1 && board2Distance < 20) && (board3Trigger == 1 && board3Distance < 20) && (board4Trigger == 1 && board4Distance < 20)) {
    bloomTo = 80;
    bloom(bloomTo);


    // } else if (board1Trigger == 1 && board1Distance < 10){
    //    bloomTo = 40;
    //    bloom(bloomTo);
    //
    // } else if (board2Trigger == 1 && board2Distance < 10){
    //    bloomTo = 5;
    //    bloom(bloomTo);
    //
  }



  else {

    unBloom(bloomTo);
  }
}


void bloom(int degree) {
  if (!bloomed) {
    for (pos = 0; pos <= degree; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
  bloomed = true;
  breathAll();
  delay(1000);
}


void unBloom(int bloomTo) {
  if (bloomed == true) {
    for (int j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, strip.Color(0, 0, 0)); // yellow with a little extra red to make it warmer
    }
    strip.show();
    for (pos = bloomTo; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    bloomed = false;
  }
}
