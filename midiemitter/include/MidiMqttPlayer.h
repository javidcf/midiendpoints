
#ifndef MIDIMQTTPLAYER_H
#define MIDIMQTTPLAYER_H

#include <b64/decode.h>
#include <mosquittopp.h>
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
class MidiMqttPlayer : private mosqpp::mosquittopp
{

  public:
    MidiMqttPlayer(const MidiMqttPlayer &) = delete;
    MidiMqttPlayer &operator=(const MidiMqttPlayer &) = delete;

    //! MIDI client name
    static const std::string MIDI_CLIENT_NAME;

    //!
    //! \brief Constructor.
    //!
    //! \param mqttServer MQTT server name or address
    //! \param mqttPort MQTT port
    //! \param mqttTopic Topic of the MQTT topic where messages are published
    //! \param clientName Client identifier for MQTT and MIDI
    //!
    MidiMqttPlayer(const std::string &mqttServer, unsigned int mqttPort,
                   const std::string &mqttTopic,
                   const std::string &clientName);

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
    //! MQTT server
    const std::string m_mqttServer;
    //! MQTT port
    const unsigned int m_mqttPort;
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

    //! MQTT callback for connection establishment
    virtual void on_connect(int rc);

    //! MQTT callback for disconnection
    virtual void on_disconnect(int rc);

    //! MQTT callback for message arrival
    virtual void on_message(const struct mosquitto_message *message);
};
}

#endif
