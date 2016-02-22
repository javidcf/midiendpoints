
#ifndef MIDIMQTTRETRANMITTER_H
#define MIDIMQTTRETRANMITTER_H

#include <b64/encode.h>
#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <mqtt/topic.h>
#include <RtMidi.h>

#include <sstream>
#include <streambuf>

namespace midiendpoints
{

//!
//! \brief Retransmits MIDI messages to a MQTT topic.
//!
//! MIDI messages are received through a Jack MIDI port and retransmitted into a
//! MQTT topic as a BSF-compliant JSON message.
class MidiMqttRetransmitter
{

public:
    MidiMqttRetransmitter(const MidiMqttRetransmitter&) =delete;
    MidiMqttRetransmitter& operator=(const MidiMqttRetransmitter&) =delete;

    //!
    //! \brief Constructor.
    //!
    //! \todo Should this be const?
    //!
    //! \param mqttServerUri URI of the MQTT server
    //! \param mqttClientId MQTT client identifier used to publish the messages
    //! \param mqttTopic Topic of the MQTT queue where messages are published
    MidiMqttRetransmitter(const std::string &mqttServerUri,
                          const std::string &mqttClientId,
                          const std::string &mqttTopic);

    //!
    //! \brief Start the retransmitter.
    //!
    //! Opens a MIDI port and connects to the MQTT server for retransmision.
    //!
    void start();

    //!
    //! \brief Stop the retransmitter.
    //!
    //! Closes any open MIDI port and MQTT connection.
    void stop();

    //!
    //! \brief Destructor.
    //!
    ~MidiMqttRetransmitter();

    //!
    //! \brief Callback for new MIDI events.
    //!
    //! \param timeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    void midiEventReceived(double timeStamp,
                           std::vector< unsigned char > * message,
                           void * userData);

private:
    //! MIDI input
    RtMidiIn m_midiIn;
    //! MQTT client
    mqtt::async_client m_mqtt;
    //! MQTT topic
    mqtt::topic m_mqttTopic;
    //! B64 encoder
    base64::encoder m_b64Encoder;
    //! String stream for message data
    std::stringstream m_message;
    // Whether the retransmitter has been started
    bool m_started;

};

}

#endif
