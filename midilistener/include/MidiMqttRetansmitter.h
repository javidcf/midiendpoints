
#ifndef MIDIMQTTRETRANMITTER_H
#define MIDIMQTTRETRANMITTER_H

#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <mqtt/topic.h>
#include <RtMidi.h>

//!
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
    //! \brief Constructor.
    //!
    //! \param mqttServerUri URI of the MQTT server
    //! \param mqttClientId MQTT client identifier used to publish the messages
    //! \param mqttTopic Topic of the MQTT queue where messages are published
    MidiMqttRetansmitter(const std::string &mqttServerUri,
                         const std::string &mqttClientId,
                         const std::string &mqttTopic);

private:
    RtMidiIn m_midiIn;
    mqtt::async_client m_mqtt;
    mqtt::topic m_mqttTopic;

    //!
    //! \brief Callback for new MIDI events.
    //!
    //! \param timeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    void midiEventReceived(double timeStamp,
                           std::vector< unsigned char > *message,
                           void * userData);

};

#endif
