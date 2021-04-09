#pragma once

#include <esphome.h>
#include "Queue.h"
#include "Payload.h"

class PayloadStorage : public Component{
public:
    PayloadStorage() : _incoming_messages(10), _outcoming_messages(10){
    }

    Queue<Payload> &get_incoming_messages_queue(){
        return _incoming_messages;
    }

    Queue<Payload> &get_outcoming_messages_queue(){
        return _outcoming_messages;
    }

private:
    Queue<Payload> _incoming_messages;
    Queue<Payload> _outcoming_messages;
};
