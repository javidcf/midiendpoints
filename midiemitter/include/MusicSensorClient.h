
#ifndef MUSICSENSORCLIENT_H
#define MUSICSENSORCLIENT_H

#include <masmusic.pb.h>
#include <MidiEndpointCommon.h>

#include <asio.hpp>
#include <asio/system_timer.hpp>
#include <bsf/SensorClient.h>
#include <log4cxx/logger.h>
#include <RtMidi.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <thread>

namespace midiendpoints
{
struct InstrumentNote
{
    int8_t instrument;
    int8_t note;
    bool operator== (const InstrumentNote& other) const {
        return this->instrument == other.instrument && this->note == other.note;
    }
};
}

namespace std {
     template <> struct hash<midiendpoints::InstrumentNote>
     {
         size_t operator()(const midiendpoints::InstrumentNote &cn) const
         {
             return (cn.instrument << sizeof(cn.note)) + cn.note;
         }
     };
}


namespace midiendpoints
{

template <typename TransportT>
using MusicSensorClientParent =
    bsf::SensorClient<TransportT, TimeSpanNoteReading, TimeSpanNoteSerializer,
                      TimeSpanNoteReadingFactory>;

//!
//! \brief Plays music messages coming from a BSF network.
//!
//! Music messages are received as protocol buffers messages from a BSF network
//! and played through a Jack MIDI port.
//!
//! \tparam TransportT BSF transport type
//!
template <typename TransportT>
class MusicSensorClient : private MusicSensorClientParent<TransportT>
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
    MusicSensorClient(const Transport &transport,
                      const typename Transport::Channel &channel,
                      const std::string &midiClientName);

    //!
    //! \brief Destructor.
    //!
    virtual ~MusicSensorClient();

    //!
    //! \brief Start the client.
    //!
    //! Opens a MIDI port and plays received messages from the network.
    //!
    void start();

    //!
    //! \brief Stop the client.
    //!
    //! Closes any open MIDI port and stops playing recevied messages from the
    //! network.
    //!
    void stop();

private:

    //! MIDI output
    RtMidiOut m_midiOut;
    //! MIDI client name
    std::string m_midiClientName;
    //! MIDI channel
    int8_t m_midiChannel;
    //! Last used MIDI channel
    int8_t m_lastUsedMidiChannel;
    //! MIDI program on each channel
    std::vector<int8_t> m_midiChannelProgram;
    //! MIDI channel used by each program
    std::vector<int8_t> m_programMidiChannel;
    //! ASIO service
    asio::io_service m_asio;
    //! ASIO work
    std::unique_ptr<asio::io_service::work> m_work;
    //! ASIO thread
    std::thread m_asioThread;
    //! A map storing timers that started a note
    std::unordered_map<InstrumentNote, std::weak_ptr<asio::system_timer>>
        m_startedNotes;
    //! Whether the retransmitter has been started
    bool m_started;

    //! Class logger
    static log4cxx::LoggerPtr LOG;
    //! \return Class logger.
    constexpr log4cxx::LoggerPtr logger() const
    {
        return MusicSensorClient<Transport>::LOG;
    }

    //!
    //! \brief Play a recevied reading.
    //!
    //! \param reading Received reading.
    //! \return true
    //!
    virtual bool onDataReading(const TimeSpanNoteReading &reading);

    //!
    //! \brief Send a MIDI ON message for a note.
    //!
    //! \param midiNote MIDI note
    //! \param velocity Note velocity
    //!
    void midiNoteOn(int8_t midiNote,
                    int8_t velocity = DEFAULT_VELOCITY);

    //!
    //! \brief Send a MIDI OFF message for a note.
    //!
    //! \param midiNote MIDI note
    //! \param velocity Note velocity
    //!
    void midiNoteOff(int8_t midiNote,
                     int8_t velocity = DEFAULT_VELOCITY);

    //!
    //! \brief Set the MIDI channel in use.
    //!
    //! \param program The new program
    //!
    void setProgram(uint8_t program);

    //!
    //! \brief Set the program for the channel in use.
    //!
    void midiSetProgram();
};
}

#include "detail/MusicSensorClient.h"

#endif
