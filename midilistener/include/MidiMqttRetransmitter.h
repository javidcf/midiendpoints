
#ifndef MIDIMQTTRETRANMITTER_H
#define MIDIMQTTRETRANMITTER_H

#include <b64/encode.h>
#include <mqtt/callback.h>
#include <mqtt/client.h>
#include <mqtt/delivery_token.h>
#include <mqtt/message.h>
#include <RtMidi.h>

#include <string>
#include <sstream>
#include <vector>

namespace midiendpoints
{

//!
//! \brief Retransmits MIDI messages to a MQTT topic.
//!
//! MIDI messages are received through a Jack MIDI port and retransmitted into a
//! MQTT topic as a BSF-compliant JSON message.
//!
class MidiMqttRetransmitter : private mqtt::callback
{

  public:
    MidiMqttRetransmitter(const MidiMqttRetransmitter &) = delete;
    MidiMqttRetransmitter &operator=(const MidiMqttRetransmitter &) = delete;

    //! MIDI client name
    static const std::string MIDI_CLIENT_NAME;

    //!
    //! \brief Constructor.
    //!
    //! \param mqttServerUri URI of the MQTT server
    //! \param mqttClientId MQTT client identifier used to publish the messagesddd
    //! \param mqttTopic Topic of the MQTT topic where messages are published
    //!
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
    //!
    void stop();

    //!
    //! \brief Destructor.
    //!
    virtual ~MidiMqttRetransmitter();

    //!
    //! \brief Callback for new MIDI events.
    //!
    //! \param timeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    //!
    void midiEventReceived(double timeStamp,
                           std::vector<unsigned char> *message, void *userData);

  private:
    //! MIDI input
    RtMidiIn m_midiIn;
    //! MQTT client
    mqtt::client m_mqtt;
    //! MQTT topic
    std::string m_mqttTopic;
    //! Base64 encoder
    base64::encoder m_b64Encoder;
    //! String stream for message data
    std::ostringstream m_message;
    //! Whether the retransmitter has been started
    bool m_started;

    //! MQTT callback for lost connection
    virtual void connection_lost(const std::string & cause);

    //! MQTT callback for new messages
    virtual void message_arrived(const std::string & topic, mqtt::message::ptr_t message);

    //! MQTT callback for completed deliveries
    virtual void delivery_complete(mqtt::idelivery_token::ptr_t token);
};
}

#endif
