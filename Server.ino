/**
 * Based on default example provided under BLE->Server
 * author Jason Hsiao
 *
 * original author unknown
 * updated by chegewara
 */
// Useful Bluetooth packages
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


// Set UIUDs for your service and any characteristics you might need using online UIUD generator: https://www.uuidgenerator.net/
#define SERVICE_UUID "b16017de-f43c-4973-a564-edc12ccbda40"
#define CHARACTERISTIC_UUID "27d92dfc-5e44-41ba-a3d0-5b5364b74a6e"
#define CHARACTERISTICTWO_UUID "39df8d8a-3e34-4531-8804-336f19ba6eef"

// These are pointers to your BLE characteristic objects
// BLE Characteristics for LED and for messaging
BLECharacteristic *pCharacteristic;
BLECharacteristic *pSecondCharacteristic;


// Define any constants or variables you might need for operating a pushbutton
#define BUTTON 4
bool buttonState = false;


// Define any constants or variables you might need for operating an LED
#define LED 7

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Monitor initialized!!");


  // Set up your pushbutton here
  pinMode(BUTTON, INPUT_PULLUP);

  // Set up your LED here
  pinMode(LED, OUTPUT);

  // Setting up your Bluetooth Device
  BLEDevice::init("Kirtan's ESP Controller");  // Replace with name of your Bluetooth device, just make this something easily identifiable so it can be found via Bluetooth scanner
  BLEServer *pServer = BLEDevice::createServer();                      // Using the BLEDevice object that you just initiated, create a Server object
  BLEService *pService = pServer->createService(SERVICE_UUID);                    // Using the Server object that you just created, create a Service with your specific UIUD that you defined in TODO #1

  // Following this trend, use the Service object you just created to create a characteristic (which we defined earlier)
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID
    , BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE
  );


  // Create a Second Characteristic using the same service to handle messaging functionality
  pSecondCharacteristic = pService->createCharacteristic(CHARACTERISTICTWO_UUID
    , BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE
  );
  // We can now start our service :)
  pService->start();

  // This part is where our Bluetooth device starts advertising its presence to surrounding receivers
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // determines the minimum gap between communications
  // pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE Set-up complete");
}

String lastmsg;
bool ledstate;
String lastMsgSent;
String recieved;

void loop() {


  String value = pSecondCharacteristic->getValue();

  if ((value != lastmsg)) {
    lastmsg = pSecondCharacteristic->getValue();
    Serial.println(value);
    if (lastmsg != lastMsgSent) {
    digitalWrite(LED,1);
    delay(1000);
    digitalWrite(LED,0);
    }

  } else if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    pCharacteristic->setValue(msg);
    lastMsgSent = msg;
  }

  delay(50);  // Small delay to reduce CPU usage
}
