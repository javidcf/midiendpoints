
#ifndef MIDIMQTTRETRANMITTER_H
#define MIDIMQTTRETRANMITTER_H

#include <b64/encode.h>
#include <mosquittopp.h>
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
class MidiMqttRetransmitter : private mosqpp::mosquittopp
{

  public:
    MidiMqttRetransmitter(const MidiMqttRetransmitter &) = delete;
    MidiMqttRetransmitter &operator=(const MidiMqttRetransmitter &) = delete;

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
    MidiMqttRetransmitter(const std::string &mqttServer,
                          unsigned int mqttPort,
                          const std::string &mqttTopic,
                          const std::string &clientName);

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
    //! \param midiTimeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    //!
    void midiEventReceived(double midiTimeStamp,
                           std::vector<unsigned char> *message, void *userData);

  private:
    //! MIDI input
    RtMidiIn m_midiIn;
    //! MQTT server
    const std::string m_mqttServer;
    //! MQTT port
    const unsigned int m_mqttPort;
    //! MQTT topic
    const std::string m_mqttTopic;
    //! Base64 encoder
    base64::encoder m_b64Encoder;
    //! String stream for message data
    std::ostringstream m_message;
    //! Whether the retransmitter has been started
    bool m_started;

    //! MQTT callback for connection establishment
    virtual void on_connect(int rc);

    //! MQTT callback for disconnection
    virtual void on_disconnect(int rc);

    //! MQTT callback for message publication
    virtual void on_publish(int mid);
};
}

#endif
