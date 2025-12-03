/**
 * Based on default example provided under BLE->Client
 * author Jason Hsiao
 *
 * original author unknown
 * updated by chegewara
 */

#define BUTTON 15
#define LED 4
#define LED2 22
#include "BLEDevice.h"

// Part 3 TODO #1: Pass in the corresponding UIUDs of the service we wish to connect to
// The remote service we wish to connect to.
static BLEUUID serviceUUID("b16017de-f43c-4973-a564-edc12ccbda40");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("27d92dfc-5e44-41ba-a3d0-5b5364b74a6e");

// Connection control booleans
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
bool connection = false;

// Pointers to the our Bluetooth objects
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteCharacteristic *pMessageRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

// Callback functions
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.write(pData, length);
  Serial.println();
  connection = true;
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  // Create a client using the BLEDevice API we used to create the server
  BLEClient *pClient = BLEDevice::createClient();
  Serial.println("Client successfully created!");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  // In the same way that we found our service, but use that Service to get our desired Characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  //Using the same remote service, obtain the second characteristic like we just did above

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  // Assuming that your characteristic will update you when it changes, how would you register/subscribe for these notifications
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback); // or something like that ;)
  }
 
  connected = true;
  return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("found");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }  // onResult
};  // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("Dev's Device"); // Not that it really matters since this device is created internally, but it doesn't hurt to rename this (at least I don't think so)

  // Initialize and set-up your button and LED here
  pinMode(LED, OUTPUT);
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //pBLEScan->setInterval(1349);
  //pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

String lastMessage;
String message;
String receive;

void loop() {

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }
 
  if (connected) {
    String val = pRemoteCharacteristic->readValue();
 
    if (!val.isEmpty()) {
      Serial.println(val.c_str());
      digitalWrite(LED, 1);
     
    } else {
      digitalWrite(LED, 0);
    }
    val = String();
  }
  delay(50);
}
