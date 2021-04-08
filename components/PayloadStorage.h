#pragma once

#include <esphome.h>
#include "Queue.h"
#include "Payload.h"

class PayloadStorage : public Component{
public:
    PayloadStorage(){
    }

    Queue<Payload> &get_incoming_messages_queue(){
        return _incoming_messages;
    }


private:
    Queue<Payload> _incoming_messages;
};
