
#ifndef MUSICSENSOR_H
#define MUSICSENSOR_H

#include <masmusic.pb.h>
#include <MidiEndpointCommon.h>

#include <bsf/Sensor.h>
#include <log4cxx/logger.h>
#include <RtMidi.h>

#include <memory>
#include <string>
#include <sstream>
#include <vector>

namespace midiendpoints
{

template <typename TransportT>
using MusicSensorParent =
    bsf::Sensor<TransportT, TimeSpanNoteReading, TimeSpanNoteSerializer,
                TimeSpanNoteReadingFactory>;

//!
//! \brief Retransmits music messages to a BSF network.
//!
//! Music messages are received through a Jack MIDI port and retransmitted
//! into a BSF network in a protocol buffers message.
//!
//! \tparam TransportT BSF transport type
//!
template <typename TransportT>
class MusicSensor : private MusicSensorParent<TransportT>
{
public:
    //! BSF transport type
    typedef TransportT Transport;

    //!
    //! \brief Constructor.
    //!
    //! \param transport BSF transport
    //! \param channel BSF transport channel
    //! \param midiClientName MIDI client identifier
    //!
    MusicSensor(const Transport &transport,
                const typename Transport::Channel &channel,
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
    //! \param midiTimeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData user data given in the callback binding
    //!
    void midiEventReceived(double midiTimeStamp,
                           std::vector<unsigned char> *message, void *userData);

private:
    enum class MidiParserStatus
    {
        WAITING_ON,
        WAITING_NOTE,
        WAITING_VELOCITY
    };

    //! MIDI input
    RtMidiIn m_midiIn;
    //! Midi client name
    std::string m_midiClientName;
    //! Current MIDI parsing status
    MidiParserStatus m_midiParserStatus;
    //! Reused reading object
    TimeSpanNoteReading m_reading;
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
    //! \param timeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData a valid pointer to a MusicSensor
    //!
    static void forwardMidiCallback(double timeStamp,
                                    std::vector<unsigned char> *message,
                                    void *userData);
};
}

#include "detail/MusicSensor.h"

#endif
