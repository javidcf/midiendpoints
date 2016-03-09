
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <log4cxx/logger.h>

#include <chrono>
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

MidiMqttRetransmitter::MidiMqttRetransmitter(const std::string &mqttServer,
                                             unsigned int mqttPort,
                                             const std::string &mqttTopic,
                                             const std::string &clientName)
: mosqpp::mosquittopp(clientName.c_str())
, m_midiIn(MIDI_API, clientName)
, m_mqttServer{mqttServer}
, m_mqttPort{mqttPort}
, m_mqttTopic{mqttTopic}
, m_b64Encoder()
, m_message()
, m_started{false}
{
    mosqpp::lib_init();
}

MidiMqttRetransmitter::~MidiMqttRetransmitter()
{
    stop();
    mosqpp::lib_cleanup();
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
        connect(m_mqttServer.data(), m_mqttPort);
        loop_start();

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
        LOG4CXX_DEBUG(logger, "Stopping listener...")
        m_started = false;
        m_midiIn.cancelCallback();
        m_midiIn.closePort();
        disconnect();
        loop_stop();
        LOG4CXX_INFO(logger, "MIDI listener stopped")
    }
}

void MidiMqttRetransmitter::midiEventReceived(
    double /*midiTimeStamp*/, std::vector<unsigned char> *message,
    void * /*userData*/)
{
    LOG4CXX_DEBUG(logger, "MIDI event received (" << message->size()
                                                  << " bytes)")

    if (message->size() > 0)
    {
        m_message.str(std::string{});

        // Get time stamp
        auto timeStamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

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
        publish(NULL, m_mqttTopic.data(), payload.size(), payload.data(),
                MQTT_QOS, MQTT_RETAIN);
    }
}

void MidiMqttRetransmitter::on_publish(int /*mid*/)
{
    LOG4CXX_DEBUG(logger, "MQTT message delivered")
}

void MidiMqttRetransmitter::on_connect(int rc)
{
    LOG4CXX_DEBUG(logger, "MQTT connection established")
    if (rc == 0)
    {
        LOG4CXX_DEBUG(logger, "MQTT connection established")
    }
    else if (m_started)
    {
        LOG4CXX_ERROR(logger, "Could not connect to MQTT server, retrying...")
        reconnect_async();
    }
}

void MidiMqttRetransmitter::on_disconnect(int rc)
{
    if (rc == 0)
    {
        if (m_started)
        {
            LOG4CXX_ERROR(logger, "MQTT connection lost, reconnecting...")
            reconnect_async();
        }
        else
        {
            LOG4CXX_DEBUG(logger, "Disconnected from MQTT server")
        }
    }
    else if (!m_started)
    {
        LOG4CXX_ERROR(logger, "Could not disconnect from MQTT server")
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
