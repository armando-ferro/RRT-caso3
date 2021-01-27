
simple SimpleNode
{
    parameters:
        volatile double pOutputLink1;
        @display("i=block/routing");
    gates:
        input gateSource;
        output gateQueue[];
        inout gateLink[];
}

simple Source
{
    parameters:
        volatile double packetGenInterval @unit(s);
        volatile double packetLength;
        @display("i=block/source");
    gates:
        output out;
}

simple FIFOQueue
{
    parameters:
        volatile double timeout @unit(s);
        @display("i=block/queue");
    gates:
        input gateNode;
        inout gateLink;
}

network Caso3
{
    types:
        channel Channel extends ned.DatarateChannel {
            delay = 5ms; // tiempo de propagación
            datarate = 9600bps;
            //per = 1e-3;
        }
    submodules:
        node[5]: SimpleNode;
        source[3]: Source;
        queue[6]: FIFOQueue;
    connections allowunconnected:
        source[0].out --> node[0].gateSource;
        source[1].out --> node[1].gateSource;
        source[2].out --> node[4].gateSource;
        
        node[0].gateQueue++ --> queue[0].gateNode;
        queue[0].gateLink <--> Channel <--> node[1].gateLink++;
        node[0].gateQueue++ --> queue[1].gateNode;
        queue[1].gateLink <--> Channel <--> node[4].gateLink++;
        
        node[1].gateQueue++ --> queue[2].gateNode;
        queue[2].gateLink <--> Channel <--> node[2].gateLink++;
        node[1].gateQueue++ --> queue[3].gateNode;
        queue[3].gateLink <--> Channel <--> node[3].gateLink++;
        
        node[4].gateQueue++ --> queue[4].gateNode;
        queue[4].gateLink <--> Channel <--> node[1].gateLink++;
        node[4].gateQueue++ --> queue[5].gateNode;
        queue[5].gateLink <--> Channel <--> node[3].gateLink++;
}

