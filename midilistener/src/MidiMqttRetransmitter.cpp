
#include "MidiMqttRetansmitter.h"

MidiMqttRetansmitter::MidiMqttRetansmitter(const std::string &mqttServerUri,
                                           const std::string &mqttClientId,
                                           const std::string &mqttTopic)
: m_midiIn ()
, m_mqtt (mqttServerUri, mqttClientId)
, m_mqttTopic (mqttTopic, m_mqtt)
{
    ;
}
