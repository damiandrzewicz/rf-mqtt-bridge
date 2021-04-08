#pragma once

#include "esphome.h"
// **********************************************************************************
#include <RFM69_ATC.h>
#include <SPI.h> // Not actually used but needed to compile
#include "Payload.h"
#include "PayloadStorage.h"

//** Configuration */


//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID        1    //should always be 1 for a Gateway
#define NETWORKID     100  //the same on all nodes that talk to each other
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_868MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
//*********************************************************************************************
//Auto Transmission Control - dials down transmit power to save battery
//Usually you do not need to always transmit at max output power
//By reducing TX power even a little you save a significant amount of battery power
//This setting enables this gateway to work with remote nodes that have ATC enabled to
//dial their power down to only the required level
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
//*********************************************************************************************

class RFM69Manager : public Component {
 public:

  RFM69Manager(PayloadStorage *payload_storage) : _radio(_SPI_CS, _IRQ_PIN) {
    this->_payload_storage = payload_storage;
  }

  void setup() override {
    ESP_LOGI(__FUNCTION__, "exec...");

    ESP_LOGI(__FUNCTION__, "Initializing RFM69HW radio: Frequency: [868MHZ], NodeId: [%d], NetworkId: [%d]",
      NODEID, NETWORKID);
    _radio.initialize(FREQUENCY,NODEID,NETWORKID);
    _radio.setHighPower(); //must include this only for RFM69HW/HCW!
    _radio.encrypt(ENCRYPTKEY);
    _radio.spyMode(false);
    ESP_LOGI(__FUNCTION__, "RFM69_ATC Enabled (Auto Transmission Control)");
  }
  void loop() override {
    receive();
    
  }

  void receive(){
    if (_radio.receiveDone())
    {
      Payload payload(reinterpret_cast<char*>(_radio.DATA), _radio.SENDERID, _radio.RSSI);
      _packetCount++;

      ESP_LOGI(__FUNCTION__, "Received: [%d][%d][%d][RX_RSSI:%d] - [%s]", 
        _packetCount, 
        payload.get_sender_id(), 
        _radio.DATALEN, 
        payload.get_rssi(), 
        payload.get_raw().c_str()
        );

      // Append payload to queue
      _payload_storage->get_incoming_messages_queue().push(payload);
      
      if (_radio.ACKRequested())
      {
        _radio.sendACK();
        ESP_LOGI(__FUNCTION__, "ACK sent");
      }
    }
  }

  void send(){
    // Check if new message in queue
    //const auto Payload &&p = _get_from_outcoming_queue();
    //ESP_LOGD(__FUNCTION__ "Sending to ")
  }

  void blink()
  {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(3);
    digitalWrite(LED_BUILTIN,LOW);
  }

private:
  //Auto Transmission Control - dials down transmit power to save battery
  //Usually you do not need to always transmit at max output power
  //By reducing TX power even a little you save a significant amount of battery power
  //This setting enables this gateway to work with remote nodes that have ATC enabled to
  //dial their power down to only the required level
  RFM69_ATC _radio;

  uint32_t _packetCount = 0;

  static const constexpr int _SPI_CS = RF69_SPI_CS;
  static const constexpr int _IRQ_PIN = 4;
  

  PayloadStorage *_payload_storage = nullptr;
};