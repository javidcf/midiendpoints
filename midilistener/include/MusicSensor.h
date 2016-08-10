
#ifndef MUSICSENSOR_H
#define MUSICSENSOR_H

#include <masmusic.pb.h>
#include <MidiEndpointCommon.h>

#include <bsf/Sensor.h>
#include <log4cxx/logger.h>
#include <RtMidi.h>

#include <chrono>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace midiendpoints
{

template <typename TransportT>
using TimePointNoteSensor =
    bsf::Sensor<TransportT, TimePointNoteReading, TimePointNoteSerializer,
                TimePointNoteReadingFactory>;
template <typename TransportT>
using TimeSpanNoteSensor =
    bsf::Sensor<TransportT, TimeSpanNoteReading, TimeSpanNoteSerializer,
                TimeSpanNoteReadingFactory>;

//!
//! \brief Retransmits music messages to a BSF network.
//!
//! Music messages are received through a Jack MIDI port and retransmitted
//! into a BSF network in a protocol buffers message. The sensor retransmits
//! two kinds of messages: instantaneous note events, which are emitted when a
//! note starts, and spanned note events, which are emitted when the note
//! finishes, and include the duration. Each kind of even can be published to
//! a different channel.
//!
//! \tparam TransportT BSF transport type
//!
template <typename TransportT>
class MusicSensor
{
public:
    //! BSF transport type
    typedef TransportT Transport;

    //!
    //! \brief Constructor.
    //!
    //! \param transport BSF transport
    //! \param channelSpanned BSF transport channel for spanned events
    //! \param channelInstant BSF transport channel for instantaneous events
    //! \param midiClientName MIDI client identifier
    //!
    MusicSensor(const Transport &transport,
                const typename Transport::Channel &channelSpanned,
                const typename Transport::Channel &channelInstant,
                const std::string &midiClientName);

    //!
    //! \brief Start the sensor.
    //!
    //! Opens a MIDI port and start publishing the music events to the network.
    //!
    void start();

    //!
    //! \brief Stop the sensor.
    //!
    //! Closes any open MIDI and stops publishing music events to the network.
    //!
    void stop();

    //!
    //! \brief Destructor.
    //!
    virtual ~MusicSensor();

    //!
    //! \brief Callback for new MIDI events.
    //!
    //! \param midiTimestamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    //!
    void midiEventReceived(double midiTimestamp,
                           std::vector<unsigned char> *message, void *userData);

private:
    enum class MidiNoteEvent
    {
        NONE,
        OFF,
        ON
    };
    enum class MidiParserStatus
    {
        WAITING,
        NOTE,
        VELOCITY
    };

    //! Maximum acceptable ON/OFF event distance
    static const unsigned int MAX_DURATION{5000};

    //! \brief Note start data.
    struct Onset
    {
        std::chrono::time_point<std::chrono::system_clock> timestamp;
        unsigned char velocity;
    };

    //! Sensor for spanned events
    TimeSpanNoteSensor<TransportT> m_sensorSpanned;
    //! Sensor for instantaneous events
    TimePointNoteSensor<TransportT> m_sensorInstant;
    //! MIDI input
    RtMidiIn m_midiIn;
    //! MIDI client name
    std::string m_midiClientName;
    //! Current MIDI note event
    MidiNoteEvent m_midiNoteEvent;
    //! Current MIDI parsing status
    MidiParserStatus m_midiParserStatus;
    //! Reused spanned reading object
    TimeSpanNoteReading m_readingSpanned;
    //! Reused instantaneous reading object
    TimePointNoteReading m_readingInstant;
    //! Current MIDI pitch value
    unsigned char m_currentPitch;
    //! Map to keep track of event start timestamps and velocites
    std::unordered_map<unsigned char, Onset> m_startedNotes;
   //! Whether the retransmitter has been started
    bool m_started;

    //! Class logger
    static log4cxx::LoggerPtr LOG;
    //! \return Class logger.
    constexpr log4cxx::LoggerPtr logger() const
    {
        return MusicSensor<Transport>::LOG;
    }

    //!
    //! \brief Forward a MIDI event callback to a MusicSensor object.
    //!
    //! This is a wrapper for the member callback function, since RtMidi does
    //! not allow to use bound functions as callback.
    //!
    //! \param timestamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData a valid pointer to a MusicSensor
    //!
    static void forwardMidiCallback(double timestamp,
                                    std::vector<unsigned char> *message,
                                    void *userData);
};
}

#include "detail/MusicSensor.h"

#endif
