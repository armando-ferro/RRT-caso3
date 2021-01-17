#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

#include "caso3_m.h"

// S&W state machine
const short idle = 0;
const short sending = 1;
const short waitingAck = 2;

class FIFOQueue : public cSimpleModule
{
    private:
        short state = -1;
        long int numQueuedPackets = -1;
    protected:
        cQueue queue;
        virtual void forwardMessage(cMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
};

Define_Module(FIFOQueue);

void FIFOQueue::initialize()
{
    state = idle;
    numQueuedPackets = 0;
}

void FIFOQueue::handleMessage(cMessage *msg)
{
    int arrivalGateId = msg->getArrivalGateId();

    if(msg->isSelfMessage()) {
        // Last data packet sent finished its transmission
        state = waitingAck;
    } else if(arrivalGateId == gate("gateNode")->getId()) {
        // Data packet arrived
        if(gate("gateLink$o")->getTransmissionChannel()->isBusy() || state != idle) {
            // If channel is busy or state is not idle, add it to queue
            queue.insert(msg);
            numQueuedPackets++;
        } else {
            // If channel is not busy and state is idle, send it
            state = sending;
            forwardMessage(msg);
        }
    } else if(arrivalGateId == gate("gateLink$i")->getId()) {
        // ACK arrived
        if(state == waitingAck) {
            if(!queue.isEmpty()) {
                cMessage *nextMsg = check_and_cast<cMessage *>(queue.pop());
                numQueuedPackets--;
                forwardMessage(nextMsg);
            } else {
                state = idle;
            }
        }
    }
}

void FIFOQueue::forwardMessage(cMessage *msg)
{
    send(msg, "gateLink$o");
    EV << "Packet sent\n";

    simtime_t finishTime = gate("gateLink$o")->getTransmissionChannel()->getTransmissionFinishTime();
    cMessage *selfmsg = new cMessage("Packet finished its transmission");
    scheduleAt(finishTime, selfmsg);
    EV << "Packet will finish its transmission at " << finishTime << "\n";
}

void FIFOQueue::refreshDisplay() const
{
    char buf[40];
    char stateString[20];
    switch(state) {
        case idle:
            sprintf(stateString, "idle");
            break;
        case sending:
            sprintf(stateString, "sending packet");
            break;
        case waitingAck:
            sprintf(stateString, "waiting ACK");
            break;
        default:
            sprintf(stateString, "unknown");
    }
    sprintf(buf, "state %s\ncontains %ld packets", stateString, numQueuedPackets);
    getDisplayString().setTagArg("t", 0, buf);
}

