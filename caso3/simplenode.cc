//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

#include "caso3_m.h"

class SimpleNode : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
    virtual void forwardMessage(Caso3Pkt *msg);
    virtual void sendACK(int gateLinkIndex);
};

Define_Module(SimpleNode);

void SimpleNode::handleMessage(cMessage *msg)
{
    Caso3Pkt *tmsg = check_and_cast<Caso3Pkt *>(msg);

    if(tmsg->getArrivalGateId() != gate("gateSource")->getId()) {
        int gateLinkIndex = tmsg->getArrivalGate()->getIndex();
        EV << "Data packet arrived at gateLink$i[" << gateLinkIndex << "]\n";
        if(tmsg->hasBitError()) {
            bubble("ARRIVED WITH ERROR. DISCARDING!");
            delete tmsg;
        } else {
            sendACK(gateLinkIndex);
            if(getIndex() != 2 && getIndex() != 3) {
                bubble("ARRIVED. ACK SENT. FORWARDING!");
                forwardMessage(tmsg);
            } else {
                bubble("ARRIVED. ACK SENT. DELETING!");
                delete tmsg;
            }
        }

    } else {
        bubble("ARRIVED FROM SOURCE. FORWARDING!");
        forwardMessage(tmsg);
    }
}

void SimpleNode::forwardMessage(Caso3Pkt *msg)
{
    int n = gateSize("gateQueue");
    int k = 0;
    // p es la probabilidad de que el paquete se envíe por la gateQueue[1]
    double p = par("pOutputLink1");

    if(n>1) {
        k = bernoulli(p);
        EV << "Forwarding message " << msg << " on port gateQueue[" << k << "]\n";
        send(msg, "gateQueue", k);
    } else {
        send(msg, "gateQueue");
    }
}

void SimpleNode::sendACK(int gateLinkIndex)
{
    while(gate("gateLink$o", gateLinkIndex)->getTransmissionChannel()->isBusy());

    bubble("SENDING ACK!");
    Caso3Pkt *ackmsg = new Caso3Pkt("ACK");
    send(ackmsg, "gateLink$o", gateLinkIndex);
}
