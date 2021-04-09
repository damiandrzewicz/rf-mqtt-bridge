#pragma once

#include <string>

class Payload
{
public:

    Payload(){}

    Payload(const std::string &raw, uint16_t device_id, int16_t rssi){
        set_raw(raw);
        set_device_id(device_id);
        set_rssi(rssi);
    }

    void set_raw(const std::string &raw){
        this->_raw = raw;
    }

    std::string get_raw() const {
        return this->_raw;
    } 

    void set_device_id(uint16_t device_id){
        this->_device_id = device_id;
    }

    uint16_t get_device_id() const {
        return this->_device_id;
    } 

    void set_rssi(int16_t rssi){
        this->_rssi = rssi;
    }

    int16_t get_rssi() const {
        return this->_rssi;
    } 
private:
    std::string _raw;
    uint16_t _device_id;
    int16_t _rssi;
};
