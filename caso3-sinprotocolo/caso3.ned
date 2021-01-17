
simple SimpleNode
{
    parameters:
        @display("i=block/routing");
    gates:
        input in[];
        output out[];
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
        @display("i=block/queue");
    gates:
        input in;
        output out;
}

network Caso3
{
    types:
        channel Channel extends ned.DatarateChannel {
            delay = 10ms; // tiempo de propagación
            datarate = 9600bps;
        }
    submodules:
        node[5]: SimpleNode;
        source[3]: Source;
        queue[6]: FIFOQueue;
    connections:
        source[0].out --> node[0].in++;
        source[1].out --> node[1].in++;
        source[2].out --> node[4].in++;
        
        node[0].out++ --> queue[0].in;
        queue[0].out --> Channel --> node[1].in++;
        node[0].out++ --> queue[1].in;
        queue[1].out --> Channel --> node[4].in++;
        
        node[1].out++ --> queue[2].in;
        queue[2].out --> Channel --> node[2].in++;
        node[1].out++ --> queue[3].in;
        queue[3].out --> Channel --> node[3].in++;
        
        node[4].out++ --> queue[4].in;
        queue[4].out --> Channel --> node[1].in++;
        node[4].out++ --> queue[5].in;
        queue[5].out --> Channel --> node[3].in++;
}

