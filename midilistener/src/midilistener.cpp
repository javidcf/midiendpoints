
#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <mqtt/topic.h>
#include <RtMidi.h>

#include <vector>


#define MQTT_SERVER_URI "tcp://localhost:1883"
#define MQTT_CLIENT_ID "midilistener"
#define MQTT_TOPIC "midi"


void midiToMqtt(double timeStamp, std::vector< unsigned char > *message, void *userData);

int main(int /*argc*/, char * /*argv*/[])
{
    // MQTT
    mqtt::async_client mqtt {MQTT_SERVER_URI, MQTT_CLIENT_ID};
    mqtt.connect();
    mqtt::topic topic {MQTT_TOPIC, mqtt};
    // topic.publish();

    RtMidiIn midiIn;

    return 0;
}

void midiToMqtt(double /*timeStamp*/, std::vector< unsigned char > *message, void * /*userData*/)
{
    mqtt::message msg {message->data(), message->size()};
}
