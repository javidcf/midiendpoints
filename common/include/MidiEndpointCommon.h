
#ifndef MIDIENDPOINTCOMMON_H
#define MIDIENDPOINTCOMMON_H

#include <masmusic.pb.h>

#if GOOGLE_PROTOBUF_VERSION >= 3000000
#define USE_PROTOBUF_V3
#endif

#ifdef USE_BASE64
#include <bsf/Base64Serializer.h>
#endif

#ifdef USE_PROTOBUF_V3
#include <bsf/ProtobufPtrDataReading.h>
#else
#include <bsf/ProtobufDataReading.h>
#endif

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <RtMidi.h>

#include <fstream>
#include <functional>
#include <memory>
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
const RtMidi::Api MIDI_API = RtMidi::UNIX_JACK; // TODO tweak this

//! MQTT quality of service
const int MQTT_QOS = 0; // TODO tune this
//! MQTT retain policy
const bool MQTT_RETAIN = false;

//! Reading type
#ifdef USE_PROTOBUF_V3
typedef bsf::ProtobufPtrDataReading<masmusic::TimePointNote> TimePointNoteReading;
typedef bsf::ProtobufArenaDataReadingFactory<masmusic::TimePointNote>
    TimePointNoteReadingFactory;
typedef bsf::ProtobufPtrDataReading<masmusic::TimeSpanNote> TimeSpanNoteReading;
typedef bsf::ProtobufArenaDataReadingFactory<masmusic::TimeSpanNote>
    TimeSpanNoteReadingFactory;
#else
typedef bsf::ProtobufDataReading<masmusic::TimePointNote> TimePointNoteReading;
typedef bsf::DefaultDataReadingFactory<masmusic::TimePointNote>
    TimePointNoteReadingFactory;
typedef bsf::ProtobufDataReading<masmusic::TimeSpanNote> TimeSpanNoteReading;
typedef bsf::DefaultDataReadingFactory<masmusic::TimeSpanNote>
    TimeSpanNoteReadingFactory;
#endif

//! Serializer type
#ifdef USE_BASE64
typedef bsf::Base64Serializer<TimePointNoteReading> TimePointNoteSerializer;
typedef bsf::Base64Serializer<TimeSpanNoteReading> TimeSpanNoteSerializer;
#else
typedef bsf::DefaultSerializer<TimePointNoteReading> TimePointNoteSerializer;
typedef bsf::DefaultSerializer<TimeSpanNoteReading> TimeSpanNoteSerializer;
#endif

//! MIDI default velocity
const unsigned char DEFAULT_VELOCITY{64};

//!
//! \brief An exception in a MIDI endpoint.
//!
class MidiEndpointException : public std::runtime_error
{
public:
    explicit MidiEndpointException(const std::string &what)
    : runtime_error(what)
    {
    }
};

//!
//! Stream buffer for an array.
//!
template <class CharT, class TraitsT = std::char_traits<CharT>>
struct membuf : std::streambuf
{
    //!
    //! Constructor.
    //!
    //! \param begin Beginning of the memory buffer
    //! \param end End of the memory buffer
    //!
    membuf(CharT *begin, CharT *end)
    {
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
template <class CharT, class TraitsT = std::char_traits<CharT>>
membuf<CharT, TraitsT> make_membuf(CharT *begin, CharT *end)
{
    return membuf<CharT, TraitsT>(begin, end);
}

//!
//! \brief Configure logging service.
//!
//! Tries to load the logging configuration from a configuration file first and
//! falls back to the default configuration if it does not exist.
//!
//! \param debug Log debug messages
//!
inline void configureLogging(bool debug = false)
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
    if (debug)
    {
        log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getDebug());
    }
    else
    {
        log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
    }
}
}

/// MasMusic helpers

namespace midiendpoints
{

//!
//! \brief Convert a pitch into a MIDI note value.
//!
//! If the pitch is out of the MIDI range then the closes unison is returned.
//!
//! \param pitch Pitch to convert
//! \return Converted MIDI note value
//!
unsigned char pitchToMidi(const masmusic::Pitch &pitch)
{
    int semitoneNumber = static_cast<int>(pitch.note());
    int octave = std::min(std::max(pitch.octave(), -1), 9);
    int midiNote = (octave + 1) * 12 + semitoneNumber;
    if (midiNote > 127)
    {
        midiNote -= 12;
    }

    return static_cast<unsigned char>(midiNote);
}

//!
//! \brief Convert a MIDI note value into a pitch.
//!
//! \param midiNote MIDI note value
//! \param pitch Converted pitch
//!
void midiToPitch(unsigned char midiNote, masmusic::Pitch *pitch)
{
    if ((midiNote & 0x80) != 0)
    {
        throw std::runtime_error("Invalid MIDI note");
    }
    auto note = static_cast<masmusic::Note>(midiNote % 12);
    pitch->set_note(note);
    pitch->set_octave(int(midiNote / 12) - 1);
}

//!
//! \brief Convert a MIDI note value into a pitch.
//!
//! \param midiNote MIDI note value
//! \return Converted pitch
//!
masmusic::Pitch midiToPitch(unsigned char midiNote)
{
    masmusic::Pitch pitch;
    midiToPitch(midiNote, &pitch);
    return pitch;
}
}

#endif
