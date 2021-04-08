#pragma once

#include <string>

class Payload
{
public:

    Payload(){}

    Payload(const std::string &raw, uint16_t sender_id, int16_t rssi){
        set_raw(raw);
        set_sender_id(sender_id);
        set_rssi(rssi);
    }

    void set_raw(const std::string &raw){
        this->_raw = raw;
    }

    std::string get_raw() const {
        return this->_raw;
    } 

    void set_sender_id(uint16_t sender_id){
        this->_sener_id = sender_id;
    }

    uint16_t get_sender_id() const {
        return this->_sener_id;
    } 

    void set_rssi(int16_t rssi){
        this->_rssi = rssi;
    }

    int16_t get_rssi() const {
        return this->_rssi;
    } 
private:
    std::string _raw;
    uint16_t _sener_id;
    int16_t _rssi;
};
