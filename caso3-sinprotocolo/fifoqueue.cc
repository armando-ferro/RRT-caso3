#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

#include "caso3_m.h"

class FIFOQueue : public cSimpleModule
{
  protected:
    cQueue queue;
    virtual void forwardMessage(cMessage *msg);
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(FIFOQueue);

void FIFOQueue::handleMessage(cMessage *msg)
{
    cGate *outputGate = gate("out");
    cChannel *outputChannel = outputGate->getTransmissionChannel();

    if(msg->isSelfMessage()) {
        if(!queue.isEmpty()) {
            cMessage *nextMsg = check_and_cast<cMessage *>(queue.pop());;
            forwardMessage(nextMsg);
        }
    } else {
        if(outputChannel->isBusy()) {
            queue.insert(msg);
        } else {
            forwardMessage(msg);
        }
    }
}

void FIFOQueue::forwardMessage(cMessage *msg)
{
    send(msg, "out");
    EV << "Packet sent\n";

    cGate *outputGate = gate("out");
    cChannel *outputChannel = outputGate->getTransmissionChannel();
    simtime_t finishTime = outputChannel->getTransmissionFinishTime();
    cMessage *selfmsg = new cMessage("Channel is free now");
    scheduleAt(finishTime, selfmsg);
    EV << "Scheduled selfmsg for " << finishTime << "\n";
}

