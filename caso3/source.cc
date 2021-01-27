#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

#include "caso3_m.h"

class Source : public cSimpleModule
{
  private:
    long numSent;
  protected:
    virtual Caso3Pkt *generatePacket(int flowSeqNum);
    virtual void forwardMessage(Caso3Pkt *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
};

Define_Module(Source);

void Source::initialize()
{
    numSent = 0;
    Caso3Pkt *msg = generatePacket(0);
    scheduleAt(0.0, msg);
}

Caso3Pkt *Source::generatePacket(int flowSeqNum)
{
    double packetLength = par("packetLength");
    int flowId;
    char msgname[20];

    if(getIndex() == 0) {
        flowId = 1;
    } else if(getIndex() == 1) {
        flowId = 2;
    } else {
        flowId = 5;
    }

    sprintf(msgname, "pkt-%d-%d", flowId, flowSeqNum);

    Caso3Pkt *msg = new Caso3Pkt(msgname);

    msg->setBitLength(packetLength);
    msg->setFlowId(flowId);
    msg->setFlowSeqNum(flowSeqNum);

    return msg;
}

void Source::handleMessage(cMessage *msg)
{
    Caso3Pkt *srcmsg = check_and_cast<Caso3Pkt *>(msg);

    if (srcmsg->isSelfMessage()) {
        Caso3Pkt *scheduledMsg = generatePacket(srcmsg->getFlowSeqNum()+1);
        simtime_t genInterval = par("packetGenInterval");

        scheduleAt(simTime()+genInterval, scheduledMsg);
        //EV << "Scheduled new message at " << simTime()+genInterval << "\n";

        srcmsg->setInitTime(simTime());
    }
    forwardMessage(srcmsg);
}

void Source::forwardMessage(Caso3Pkt *msg)
{
    //EV << "Forwarding message " << msg << "\n";
    numSent++;
    send(msg, "out");
}

void Source::refreshDisplay() const
{
    char buf[200];
    sprintf(buf, "sent: %ld", numSent);
    getDisplayString().setTagArg("t", 0, buf);
}
