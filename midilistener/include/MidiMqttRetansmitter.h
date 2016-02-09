
#ifndef MIDIMQTTRETRANMITTER_H
#define MIDIMQTTRETRANMITTER_H

#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <mqtt/topic.h>
#include <RtMidi.h>

// #include <memory>

//! \brief Retransmits MIDI messages to a MQTT topic.
//!
//! MIDI messages are received through a Jack MIDI port and retransmitted into a
//! MQTT topic as a JSON message.
class MidiMqttRetansmitter
{
public:
    MidiMqttRetansmitter(const MidiMqttRetansmitter&) =delete;
    MidiMqttRetansmitter& operator=(const MidiMqttRetansmitter&) =delete;

    //!
    MidiMqttRetansmitter(const std::string &mqttServerUri,
                         const std::string &mqttClientId,
                         const std::string &mqttTopic);
private:
    RtMidiIn m_midiIn;
    mqtt::async_client m_mqtt;
    mqtt::topic m_mqttTopic;
};

#endif
