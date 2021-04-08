#pragma once

#include "esphome.h"

#include "Payload.h"
#include "PayloadStorage.h"
#include <functional>

class MqttClient : public Component, public CustomMQTTDevice{
public:

    MqttClient(PayloadStorage *payload_storage){
        this->_payload_storage = payload_storage;
    }

    void setup() override{
        ESP_LOGD(__FUNCTION__, "Starting [MqttDispatcher]...");
        subscribe("test/message", &MqttClient::on_test_message);
        subscribe_json("test/json_message", &MqttClient::on_test_json_message);
    }

    void loop() override{

        if(!_payload_storage->get_incoming_messages_queue().empty()){
            ESP_LOGD(__FUNCTION__, "Sending message to MQTT broker...");
            ESP_LOGD(__FUNCTION__, "storage before: %d", _payload_storage->get_incoming_messages_queue().count());
            Payload p = _payload_storage->get_incoming_messages_queue().pop();
            publish(
                buildTopic(p.get_sender_id()), 
                p.get_raw());
            ESP_LOGD(__FUNCTION__, "storage after: %d", _payload_storage->get_incoming_messages_queue().count());
        }
    }

    std::string buildTopic(const uint16_t sender_id) const {
        return _topic_base + esphome::to_string(sender_id);
    }

    void on_test_message(const std::string &payload){
        ESP_LOGD(__FUNCTION__, "Received test message: [%s]", payload.c_str());
    }

    void on_test_json_message(JsonObject &root){
        if (!root.containsKey("key"))
            return;

        int value = root["key"];
        ESP_LOGD(__FUNCTION__, "Received test JSON message: [key]=[%d]", value);
    }

private:
    PayloadStorage *_payload_storage = nullptr;

    const std::string _topic_base = "rf868_bridge/data/";
};
