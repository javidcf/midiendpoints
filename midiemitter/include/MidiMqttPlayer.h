
#ifndef MIDIMQTTPLAYER_H
#define MIDIMQTTPLAYER_H

#include <b64/decode.h>
#include <mqtt/callback.h>
#include <mqtt/client.h>
#include <mqtt/delivery_token.h>
#include <mqtt/message.h>
#include <RtMidi.h>

#include <sstream>
#include <string>
#include <vector>

namespace midiendpoints
{

//!
//! \brief Plays MIDI messages coming from a MQTT topic.
//!
//! MIDI messages are received as BSF-compliant JSON messages from a MQTT topic
//! and played through a Jack MIDI port.
//!
class MidiMqttPlayer : private mqtt::callback
{

  public:
    MidiMqttPlayer(const MidiMqttPlayer &) = delete;
    MidiMqttPlayer &operator=(const MidiMqttPlayer &) = delete;

    //! MIDI client name
    static const std::string MIDI_CLIENT_NAME;

    //!
    //! \brief Constructor.
    //!
    //! \param mqttServerUri URI of the MQTT server
    //! \param mqttClientId MQTT client identifier used to subscribe to the
    //!                     messages
    //! \param mqttTopic Topic of the MQTT topic to subscribe
    //!
    MidiMqttPlayer(const std::string &mqttServerUri,
                   const std::string &mqttClientId,
                   const std::string &mqttTopic);

    //!
    //! \brief Start the player.
    //!
    //! Opens a MIDI port and connects to the MQTT server for playing.
    //!
    void start();

    //!
    //! \brief Stop the player.
    //!
    //! Closes any open MIDI port and MQTT connection.
    //!
    void stop();

    //!
    //! \brief Destructor.
    //!
    virtual ~MidiMqttPlayer();

  private:
    //! MIDI output
    RtMidiOut m_midiOut;
    //! MQTT client
    mqtt::client m_mqtt;
    //! MQTT topic
    const std::string m_mqttTopic;
    //! Base64 decoder
    base64::decoder m_b64Decoder;
    //! Base64 decoded data
    std::ostringstream m_b64Decoded;
    //! MIDI message data
    std::vector<unsigned char> m_midiMessage;
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
