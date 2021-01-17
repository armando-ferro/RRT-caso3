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
};

Define_Module(SimpleNode);

void SimpleNode::handleMessage(cMessage *msg)
{
    Caso3Pkt *tmsg = check_and_cast<Caso3Pkt *>(msg);

    if(getIndex() != 2 && getIndex() != 3) {
        bubble("ARRIVED, FORWARDING!");
        forwardMessage(tmsg);
    } else {
        bubble("ARRIVED, DELETING!");
        delete tmsg;
    }
}

void SimpleNode::forwardMessage(Caso3Pkt *msg)
{
    int n = gateSize("out");
    int k = 0;

    if(n>1) {
        k = intuniform(0, n-1);
        EV << "Forwarding message " << msg << " on port out[" << k << "]\n";
        send(msg, "out", k);
    } else {
        send(msg, "out");
    }
}
