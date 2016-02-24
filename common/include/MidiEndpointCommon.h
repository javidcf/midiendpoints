
#ifndef MIDIENDPOINTCOMMON_H
#define MIDIENDPOINTCOMMON_H

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <RtMidi.h>

#include <fstream>
#include <stdexcept>
#include <streambuf>
#include <string>

#ifndef NDEBUG
#define DEBUG 1
#endif

namespace midiendpoints
{

//! Logger configuration file path
const std::string LOGGER_CONF_PATH{"/etc/midiendpoints/logging.conf"};

//! RtMidi API
const RtMidi::Api MIDI_API = RtMidi::UNIX_JACK;  // TODO tweak this

//! MQTT quality of service
const int MQTT_QOS = 1;  // TODO tune this
//! MQTT retain policy
const bool MQTT_RETAIN = false;

//! BSF JSON message begin mark
const std::string BSF_JSON_BEGIN{"<JSON>"};
//! BSF JSON message end mark
const std::string BSF_JSON_END{"</JSON>"};

//! JSON timestamp field
const std::string JSON_TIMESTAMP{"at"};
//! JSON event data field
const std::string JSON_EVENT_DATA{"data"};

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
    //!
    //! Constructor.
    //!
    //! \param begin Beginning of the memory buffer
    //! \param end End of the memory buffer
    //!
    membuf(CharT * begin, CharT * end) {
        this->setg(begin, begin, end);
    }
};

//!
//! \brief Create a memory buffer.
//!
//! \param begin Beginning of the memory buffer
//! \param end End of the memory buffer
//! \return A memory buffer for the given segment of memory.
//!
template<class CharT, class TraitsT = std::char_traits< CharT > >
membuf<CharT, TraitsT> make_membuf(CharT * begin, CharT * end)
{
    return membuf<CharT, TraitsT>(begin, end);
}

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
    #if DEBUG
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getDebug());
    #else
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
    #endif
}

}

#endif
