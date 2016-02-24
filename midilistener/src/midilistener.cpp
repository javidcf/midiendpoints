
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <log4cxx/logger.h>

#include <stdexcept>

#define MQTT_SERVER_URI "tcp://localhost:1883"
#define MQTT_CLIENT_ID "midilistener"
#define MQTT_TOPIC "midi"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("midiemitter"));

int main(int /*argc*/, char * /*argv*/ [])
{
    using namespace midiendpoints;

    configureLogging();

    try
    {
        MidiMqttRetransmitter retransmitter(MQTT_SERVER_URI, MQTT_CLIENT_ID,
                                            MQTT_TOPIC);
        retransmitter.start();

        // TODO make this right
        LOG4CXX_INFO(logger, "Reading MIDI input, press <enter> to quit...")
        char input;
        std::cin.get(input);
    }
    catch (std::exception &e)
    {
        LOG4CXX_ERROR(logger, e.what())
    }

    return 0;
}
