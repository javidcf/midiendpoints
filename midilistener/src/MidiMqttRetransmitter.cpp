
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <log4cxx/logger.h>

#include <istream>

namespace midiendpoints
{

static log4cxx::LoggerPtr
    logger(log4cxx::Logger::getLogger("MidiMqttRetransmitter"));

const std::string MidiMqttRetransmitter::MIDI_CLIENT_NAME{"midilistener"};

//!
//! \brief Forward a MIDI event callback to a MidiMqttRetransmitter object.
//!
//! This is a wrapper for the member callback function, since RtMidi does not
//! allow to use bound functions as callback.
//!
//! \param timeStamp time stamp of the MIDI event
//! \param message MIDI event data
//! \param userData a valid pointer to a MidiMqttRetransmitter
//!
void forwardMidiMqttRetransmitterEventCallback(
    double timeStamp, std::vector<unsigned char> *message, void *userData);

MidiMqttRetransmitter::MidiMqttRetransmitter(const std::string &mqttServerUri,
                                             const std::string &mqttClientId,
                                             const std::string &mqttTopic)
: m_midiIn(MIDI_API, MIDI_CLIENT_NAME)
, m_mqtt(mqttServerUri, mqttClientId)
, m_mqttTopic(mqttTopic)
, m_b64Encoder()
, m_message()
, m_started{false}
{
    m_mqtt.set_callback(*this);
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
        m_midiIn.openPort(0, "midi_in");
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
        if (m_mqtt.is_connected())
        {
            m_mqtt.disconnect();
        }
        m_started = false;
    }
}

void MidiMqttRetransmitter::midiEventReceived(
    double timeStamp, std::vector<unsigned char> *message, void * /*userData*/)
{
    LOG4CXX_DEBUG(logger, "MIDI event received at " << timeStamp << " ("
                                                    << message->size()
                                                    << " bytes)")

    if (message->size() > 0)
    {
        m_message.str(std::string{});

        // Not using JSON library here (for now at least)
        m_message << BSF_JSON_BEGIN << "{"
                  << "\"" << JSON_TIMESTAMP << "\":" << timeStamp << ","
                  << "\"" << JSON_EVENT_DATA << "\":\"";
        // Encode MIDI event in B64
        auto data = reinterpret_cast<char *>(message->data());
        auto dataBuf = make_membuf(data, data + message->size());
        std::istream dataStream(&dataBuf);
        m_b64Encoder.encode(dataStream, m_message);
        // Remove trailing newline from base64 encoder
        m_message.seekp(-1, std::ios_base::end);
        // Finish message
        m_message << "\"}" << BSF_JSON_END;
        auto payload = m_message.str();

        LOG4CXX_DEBUG(logger, "Publishing MQTT message")
        m_mqtt.publish(m_mqttTopic, payload.c_str(), payload.size(), MQTT_QOS,
                       MQTT_RETAIN);
    }
}

void MidiMqttRetransmitter::message_arrived(const std::string &topic,
                                     mqtt::message::ptr_t /*message*/)
{
    LOG4CXX_ERROR(logger, "Unexpected MQTT message received on topic '" << topic << "'")
}

void MidiMqttRetransmitter::connection_lost(const std::string & /*cause*/)
{
    // TODO handle error? raise exception?
    LOG4CXX_ERROR(logger, "MQTT connection lost")
}

void MidiMqttRetransmitter::delivery_complete(mqtt::idelivery_token::ptr_t token)
{
    if (token && token->get_message())
    {
        LOG4CXX_DEBUG(logger, "MQTT message delivered ("
                                  << token->get_message()->get_payload().size()
                                  << " bytes)")
    }
}

void forwardMidiMqttRetransmitterEventCallback(
    double timeStamp, std::vector<unsigned char> *message, void *userData)
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
