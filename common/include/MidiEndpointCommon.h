
#ifndef MIDIENDPOINTCOMMON_H
#define MIDIENDPOINTCOMMON_H

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <RtMidi.h>

#include <fstream>
#include <stdexcept>
#include <streambuf>

namespace midiendpoints
{

//! Logger configuration file path
const char * const LOGGER_CONF_PATH = "/etc/midiendpoints/logging.conf";

//! RtMidi API
const RtMidi::Api MIDI_API = RtMidi::UNIX_JACK;  // TODO tweak this

//! MQTT quality of service
const int MQTT_QOS = 1;
//! MQTT retain policy
const bool MQTT_RETAIN = false;

//! BSF JSON message begin mark
const char * const BSF_JSON_BEGIN = "<JSON>";
//! BSF JSON message end mark
const char * const BSF_JSON_END = "</JSON>";

//! JSON timestamp field
const char * const JSON_TIMESTAMP = "at";
//! JSON event data field
const char * const JSON_EVENT_DATA = "data";

//!
//! \brief An exception in a MIDI endpoint.
//!
class MidiEndpointException : public std::runtime_error
{
public:
    explicit MidiEndpointException (const std::string& what) : runtime_error(what) {}
};

//!
//! Stream buffer for an array.
//!
template<class CharT, class TraitsT = std::char_traits< CharT > >
struct membuf : std::streambuf
{
    membuf(CharT * begin, CharT * end) {
        this->setg(begin, begin, end);
    }
};

//!
//! \brief Configure logging service.
//!
//! Tries to load the logging configuration from a configuration file first and
//! falls back to the default configuration if it does not exist.
//!
inline void configureLogging()
{
    std::ifstream confFile(LOGGER_CONF_PATH);
    if (confFile.good())
    {
        log4cxx::PropertyConfigurator(LOGGER_CONF_PATH);
    }
    else
    {
        log4cxx::BasicConfigurator::configure();
    }
}

}

#endif
