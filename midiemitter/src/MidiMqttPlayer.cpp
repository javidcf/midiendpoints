
#include "MidiEndpointCommon.h"
#include "MidiMqttPlayer.h"

#include <json.hpp>
#include <log4cxx/logger.h>

#include <algorithm>
#include <cassert>
#include <sstream>

using json = nlohmann::json;

namespace midiendpoints
{

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("MidiMqttPlayer"));

const std::string MidiMqttPlayer::MIDI_CLIENT_NAME{"midiemitter"};

MidiMqttPlayer::MidiMqttPlayer(const std::string &mqttServer,
                               unsigned int mqttPort,
                               const std::string &mqttTopic,
                               const std::string &clientName)
: mosqpp::mosquittopp(clientName.c_str())
, m_midiOut(MIDI_API, clientName)
, m_mqttServer{mqttServer}
, m_mqttPort{mqttPort}
, m_mqttTopic{mqttTopic}
, m_b64Decoder()
, m_midiMessage()
, m_started{false}
{
    mosqpp::lib_init();
}

MidiMqttPlayer::~MidiMqttPlayer()
{
    stop();
    mosqpp::lib_cleanup();
}

void MidiMqttPlayer::start()
{
    if (!m_started)
    {
        LOG4CXX_DEBUG(logger, "Checking MIDI ports...")
        // Check MIDI ports
        if (m_midiOut.getPortCount() < 1)
        {
            throw MidiEndpointException("No MIDI ports available");
        }

        // MQTT connection
        connect(m_mqttServer.data(), m_mqttPort);
        loop_start();
        subscribe(NULL, m_mqttTopic.data(), MQTT_QOS);

        // MIDI output
        LOG4CXX_DEBUG(logger, "Opening MIDI port...")
        m_midiOut.openPort(0, "midi_out");

        m_started = true;
        LOG4CXX_INFO(logger, "MIDI emitter started")
    }
    else
    {
        LOG4CXX_WARN(logger, "MIDI emitter was already started")
    }
}

void MidiMqttPlayer::stop()
{
    if (m_started)
    {
        LOG4CXX_DEBUG(logger, "Stopping emitter...")
        m_started = false;
        m_midiOut.closePort();
        unsubscribe(NULL, m_mqttTopic.data());
        disconnect();
        loop_stop();
        LOG4CXX_INFO(logger, "MIDI emitter stopped")
    }
}

void MidiMqttPlayer::on_message(const struct mosquitto_message *message)
{
    const std::string payload(
        std::string(reinterpret_cast<const char *>(message->payload), message->payloadlen));
    LOG4CXX_DEBUG(logger, "MQTT message arrived (" << payload.size()
                                                   << " bytes):\n" << payload)

    // Check format
    if (payload.compare(0, BSF_JSON_BEGIN.size(), BSF_JSON_BEGIN) ||
        payload.compare(payload.size() - BSF_JSON_END.size(),
                        BSF_JSON_END.size(), BSF_JSON_END))
    {
        LOG4CXX_ERROR(logger, "Invalid MQTT message")
        return;
    }

    // Parse JSON
    auto &&payloadJson = payload.substr(BSF_JSON_BEGIN.size(),
                                        payload.size() - BSF_JSON_BEGIN.size() -
                                            BSF_JSON_END.size());
    auto jsonDoc = json::parse(std::istringstream(payloadJson));

    // Decode base64 data
    const std::string &b64Encoded = jsonDoc[JSON_EVENT_DATA];
    std::istringstream b64EncodedStream(b64Encoded);
    m_b64Decoded.str(std::string{});
    m_b64Decoder.decode(b64EncodedStream, m_b64Decoded);
    auto b64Data = m_b64Decoded.str();

    // Copy to message vector
    m_midiMessage.clear();
    std::copy(b64Data.begin(), b64Data.end(),
              std::back_inserter(m_midiMessage));

    LOG4CXX_DEBUG(logger, "MIDI event received at "
                              << jsonDoc[JSON_TIMESTAMP] << " ("
                              << m_midiMessage.size() << " bytes)")

    m_midiOut.sendMessage(&m_midiMessage);
}

void MidiMqttPlayer::on_connect(int rc)
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

void MidiMqttPlayer::on_disconnect(int rc)
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
}
