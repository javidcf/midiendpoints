
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

MidiMqttPlayer::MidiMqttPlayer(const std::string &mqttServerUri,
                               const std::string &mqttClientId,
                               const std::string &mqttTopic)
: m_midiOut(MIDI_API, MIDI_CLIENT_NAME)
, m_mqtt(mqttServerUri, mqttClientId)
, m_mqttTopic(mqttTopic)
, m_b64Decoder()
, m_midiMessage()
, m_started{false}
{
    m_mqtt.set_callback(*this);
}

MidiMqttPlayer::~MidiMqttPlayer()
{
    stop();
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
        LOG4CXX_DEBUG(logger, "Connecting to MQTT...")
        m_mqtt.connect();
        LOG4CXX_DEBUG(logger, "Subscribing to MQTT topic...")
        m_mqtt.subscribe(m_mqttTopic, MQTT_QOS);

        // MIDI output
        LOG4CXX_DEBUG(logger, "Opening MIDI port...")
        m_midiOut.openPort(0, "midi_out");

        m_started = true;
        LOG4CXX_INFO(logger, "MIDI player started")
    }
    else
    {
        LOG4CXX_WARN(logger, "MIDI player was already started")
    }
}

void MidiMqttPlayer::stop()
{
    if (m_started)
    {
        m_midiOut.closePort();
        m_mqtt.unsubscribe(m_mqttTopic);
        if (m_mqtt.is_connected())
        {
            m_mqtt.disconnect();
        }
        m_started = false;
    }
}

void MidiMqttPlayer::message_arrived(const std::string &topic,
                                     mqtt::message::ptr_t message)
{
    LOG4CXX_ASSERT(logger, topic == m_mqttTopic, "Unexpected MQTT topic '"
                                                     << topic << "'")

    const auto &payload = message->get_payload();
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

void MidiMqttPlayer::connection_lost(const std::string & /*cause*/)
{
    // TODO handle error? raise exception?
    LOG4CXX_ERROR(logger, "MQTT connection lost")
}

void MidiMqttPlayer::delivery_complete(mqtt::idelivery_token::ptr_t token)
{
    if (token && token->get_message())
    {
        LOG4CXX_ERROR(logger, "Unexpected MQTT message delivery ("
                                  << token->get_message()->get_payload().size()
                                  << " bytes)")
    }
}
}
