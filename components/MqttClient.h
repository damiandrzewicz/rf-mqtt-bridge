#pragma once

#include <esphome.h>
#include <ArduinoJson.h>

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
        subscribe_json(build_topic_command(), &MqttClient::on_command);
    }

    void loop() override{

        if(!_payload_storage->get_incoming_messages_queue().empty()){
            ESP_LOGD(__FUNCTION__, "Sending message to MQTT broker...");
            ESP_LOGD(__FUNCTION__, "storage before: %d", _payload_storage->get_incoming_messages_queue().count());
            Payload p = _payload_storage->get_incoming_messages_queue().pop();
            publish(
                build_topic_state(p.get_sender_id()), 
                build_payload_state(p));
            ESP_LOGD(__FUNCTION__, "storage after: %d", _payload_storage->get_incoming_messages_queue().count());
        }
    }

    std::string build_topic_state(const uint16_t sender_id) const {
        return _topic_base + "/" + _topic_state + "/" + esphome::to_string(sender_id);
    }

    std::string build_payload_state(const Payload &p){
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["senderId"] = p.get_sender_id();
        root["rssi"] = p.get_rssi();
        root["rawLen"] = p.get_raw().length();
        root["raw"] = p.get_raw();
        
        std::string buffer;
        root.printTo(buffer);
        return buffer;
    }

    std::string build_topic_command() const {
        return _topic_base + "/" + _topic_command + "/+";
    }

    void set_topic_base(const std::string &base){
        this->_topic_base = base;
    }

    void set_topic_state(const std::string &state){
        this->_topic_state = state;
    }

    void set_topic_command(const std::string &command){
        this->_topic_command = command;
    }

    void on_command(const std::string &topic, JsonObject &root){
        std::string payload;
        root.printTo(payload);
        ESP_LOGD(__FUNCTION__, "Received JSON message: topic=[%s], payload=[%s]", topic.c_str(), payload.c_str());
    }

private:
    PayloadStorage *_payload_storage = nullptr;

    std::string _topic_base = "rf868_bridge";
    std::string _topic_state = "state";
    std::string _topic_command = "command";
};
