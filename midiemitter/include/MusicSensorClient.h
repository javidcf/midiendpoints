
#ifndef MUSICSENSORCLIENT_H
#define MUSICSENSORCLIENT_H

#include <masmusic.pb.h>
#include <MidiEndpointCommon.h>

#include <asio.hpp>
#include <bsf/SensorClient.h>
#include <log4cxx/logger.h>
#include <RtMidi.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

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

    //!
    //! \brief Get the MIDI channel in use.
    //!
    //! \return The MIDI channel
    //!
    unsigned char getMidiChannel() const
    {
        return m_midiChannel;
    }

    //!
    //! \brief Set the MIDI channel in use.
    //!
    //! \param midiChannel The new MIDI channel
    //!
    void setMidiChannel(unsigned char midiChannel)
    {
        if (midiChannel >= 16)
        {
            throw std::runtime_error("Invalid MIDI channel value");
        }
        m_midiChannel = midiChannel;
        setMidiProgram();
    }

    //!
    //! \brief Get the MIDI program in use.
    //!
    //! \return The MIDI program
    //!
    unsigned char getMidiProgram() const
    {
        return m_midiProgram;
    }

    //!
    //! \brief Set the MIDI program in use.
    //!
    //! \param midiProgram The new MIDI program
    //!
    void setMidiProgram(unsigned char midiProgram)
    {
        m_midiProgram = midiProgram;
        setMidiProgram();
    }

private:
    //! MIDI output
    RtMidiOut m_midiOut;
    //! MIDI channel
    unsigned char m_midiChannel;
    //! MIDI program
    unsigned char m_midiProgram;
    //! ASIO service
    asio::io_service m_asio;
    //! ASIO work
    std::unique_ptr<asio::io_service::work> m_work;
    //! ASIO thread
    std::thread m_asioThread;
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
    void midiNoteOn(unsigned char midiNote,
                    unsigned char velocity = DEFAULT_VELOCITY);

    //!
    //! \brief Send a MIDI OFF message for a note.
    //!
    //! \param midiNote MIDI note
    //! \param velocity Note velocity
    //!
    void midiNoteOff(unsigned char midiNote,
                     unsigned char velocity = DEFAULT_VELOCITY);

    //!
    //! \brief Set the currently selected program in the channel in use.
    //!
    void midiSetProgram();
};
}

#include "detail/MusicSensorClient.h"

#endif
