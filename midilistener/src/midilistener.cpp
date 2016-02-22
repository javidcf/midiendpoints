
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <log4cxx/basicconfigurator.h>

#define MQTT_SERVER_URI "tcp://localhost:1883"
#define MQTT_CLIENT_ID "midilistener"
#define MQTT_TOPIC "midi"


int main(int /*argc*/, char * /*argv*/[])
{
    using namespace midiendpoints;

    configureLogging();

    MidiMqttRetransmitter retransmitter(MQTT_SERVER_URI,
                                        MQTT_CLIENT_ID,
                                        MQTT_TOPIC);
    retransmitter.start();

    // TODO make this right
    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);

    return 0;
}
