
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <log4cxx/logger.h>

#include <istream>

namespace midiendpoints
{

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("MidiMqttRetransmitter"));

//!
//! \brief Forward a MIDI event callback to a MidiMqttRetransmitter object.
//!
//! This is a wrapper for the member callback function, since RtMidi does not
//! allow to use bound functions as callback.
//!
//! \param timeStamp time stamp of the MIDI event
//! \param message MIDI event data
//! \param userData a valid pointer to a MidiMqttRetransmitter
void forwardMidiMqttRetransmitterEventCallback(double timeStamp,
                                               std::vector< unsigned char > *message,
                                               void * userData);

MidiMqttRetransmitter::MidiMqttRetransmitter(const std::string &mqttServerUri,
                                             const std::string &mqttClientId,
                                             const std::string &mqttTopic)
: m_midiIn(MIDI_API)
, m_mqtt(mqttServerUri, mqttClientId)
, m_mqttTopic(mqttTopic, m_mqtt)
, m_b64Encoder()
, m_message()
, m_started{false}
{
}

MidiMqttRetransmitter::~MidiMqttRetransmitter()
{
    stop();
}

void MidiMqttRetransmitter::start()
{
    if (!m_started)
    {
        // Check MIDI ports
        if (m_midiIn.getPortCount() < 1)
        {
            throw MidiEndpointException("No MIDI ports available");
        }

        // MQTT connection
        m_mqtt.connect();

        // MIDI input
        m_midiIn.openPort(0);
        m_midiIn.setCallback(forwardMidiMqttRetransmitterEventCallback, this);
        // midiin->ignoreTypes( false, false, false );

        m_started = true;
        LOG4CXX_INFO(logger, "MIDI listener started")
    }
    else
    {
        LOG4CXX_WARN(logger, "MIDI listener was already started")
    }
}

void MidiMqttRetransmitter::stop()
{
    if (m_started)
    {
        m_midiIn.cancelCallback();
        m_midiIn.closePort();
        m_mqtt.disconnect();
        m_started = false;
    }
}

void MidiMqttRetransmitter::midiEventReceived(double timeStamp,
                                              std::vector< unsigned char > * message,
                                              void * /*userData*/)
{
    LOG4CXX_DEBUG(logger, "MIDI event received at " << timeStamp << " (" << message->size() << " bytes)")

    if (message->size() > 0)
    {
        m_message.str(std::string{});

        // Not using JSON library (for now at least)
        m_message << BSF_JSON_BEGIN << "{"
                  << "\"" << JSON_TIMESTAMP << "\":" << timeStamp << ","
                  << "\"" << JSON_EVENT_DATA << "\":\"";
        // Encode MIDI event in B64
        char * data = reinterpret_cast<char *>(message->data());
        membuf<char> dataBuf(data, data + message->size());
        std::istream dataStream(&dataBuf);
        m_b64Encoder.encode(dataStream, m_message);
        // Remove trailing newline from base64 encoder
        m_message.seekp(-1, std::ios_base::end);
        // Finish message
        m_message << "\"}" << BSF_JSON_END;

        m_mqttTopic.publish(m_message.str(), MQTT_QOS, MQTT_RETAIN);
    }
}

void forwardMidiMqttRetransmitterEventCallback(double timeStamp,
                                               std::vector< unsigned char > * message,
                                               void * userData)
{
   auto retransmitter = static_cast<MidiMqttRetransmitter *>(userData);
   if (retransmitter)
   {
       retransmitter->midiEventReceived(timeStamp, message, userData);
   }
   else
   {
       LOG4CXX_ERROR(logger, "Invalid pointer in MIDI event callback")
   }
}

}
