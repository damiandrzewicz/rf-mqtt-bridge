#pragma once

#include <esphome.h>
#include <ArduinoJson.h>

#include "Payload.h"
#include "PayloadStorage.h"
#include <functional>

class MqttClient : public Component, public CustomMQTTDevice{
public:

    MqttClient(/*PayloadStorage *payload_storage*/){
        //this->_payload_storage = payload_storage;
    }

    void setRfm69Sender(std::function<void(const Payload&)> ptr){
        this->rfm69sender = ptr;
    }

    void setup() override{
        ESP_LOGI(__FUNCTION__, "Starting [MqttClient]...");

        const std::string command_topic = build_command_topic_all_ids();
        ESP_LOGI(__FUNCTION__, "Subscribing topic [%s]", command_topic.c_str());
        subscribe_json(command_topic, &MqttClient::on_command);
    }

    void send(const Payload& p){
        const std::string topic = build_state_topic(p.get_device_id());
        const std::string payload = build_state_payload(p);

        ESP_LOGI(__FUNCTION__, "Sending message to MQTT broker... topic=[%s]. payload=[%s]", topic.c_str(), payload.c_str());

        if(publish(topic, payload)){
            ESP_LOGI(__FUNCTION__, "MQTT message published!");
        } else {
            ESP_LOGW(__FUNCTION__, "MQTT message publish failed!");
        }
    }

    // void loop() override{
    //     if(!_payload_storage->get_incoming_messages_queue().empty()){
    //         Payload p = _payload_storage->get_incoming_messages_queue().pop();
    //         const std::string topic = build_state_topic(p.get_device_id());
    //         const std::string payload = build_state_payload(p);

    //         ESP_LOGI(__FUNCTION__, "Sending message to MQTT broker... topic=[%s]. payload=[%s]", topic.c_str(), payload.c_str());

    //         if(publish(topic, payload)){
    //             ESP_LOGI(__FUNCTION__, "MQTT message published!");
    //         } else {
    //             ESP_LOGW(__FUNCTION__, "MQTT message publish failed!");
    //         }
    //     }
    // }

    std::string build_state_topic(const uint16_t sender_id) const {
        return _topic_base + "/" + _topic_state + "/" + esphome::to_string(sender_id);
    }

    std::string build_state_payload(const Payload &p){
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["sourceId"] = p.get_device_id();
        root["rssi"] = p.get_rssi();
        root["rawSize"] = p.get_raw().length();
        root["raw"] = p.get_raw();
        
        std::string buffer;
        root.printTo(buffer);
        return buffer;
    }

    std::string build_command_topic_all_ids() const {
        return _topic_base + "/" + _topic_command + "/+";
    }

    void set_topic_base(const std::string &base){
        this->_topic_base = base;
    }

    void set_topic_state(const std::string &state){
        this->_topic_state = state;
    }

    void on_command(const std::string &topic, JsonObject &root){
        std::string payload;
        root.printTo(payload);
        ESP_LOGI(__FUNCTION__, "Received JSON message: topic=[%s], payload=[%s]", topic.c_str(), payload.c_str());

        if(!root.containsKey("targetId") || !root.containsKey("raw")){
            ESP_LOGW(__FUNCTION__, "MQTT payload not valid!");
            return;
        }

        Payload p;
        p.set_device_id(root["targetId"]);
        p.set_raw(root["raw"]);

        rfm69sender(p);
        //_payload_storage->get_outcoming_messages_queue().push(p);
    }

private:
    //PayloadStorage *_payload_storage = nullptr;
    std::function<void(const Payload&)> rfm69sender = nullptr;

    std::string _topic_base = "rf_mqtt_bridge";
    std::string _topic_state = "state";
    std::string _topic_command = "command";
};
