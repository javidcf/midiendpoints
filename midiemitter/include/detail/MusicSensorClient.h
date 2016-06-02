
#ifndef MUSICSENSORCLIENT_DETAIL_H
#define MUSICSENSORCLIENT_DETAIL_H

#include "../MusicSensorClient.h"

#include <algorithm>
#include <chrono>
#include <vector>

namespace midiendpoints
{

template <typename TransportT>
log4cxx::LoggerPtr MusicSensorClient<TransportT>::LOG(
    log4cxx::Logger::getLogger("MusicSensorClient"));

template <typename TransportT>
MusicSensorClient<TransportT>::MusicSensorClient(
    const Transport &transport, const typename Transport::Channel &channel,
    const std::string &midiClientName)
: MusicSensorClientParent<TransportT>(transport, channel)
, m_midiOut(MIDI_API, midiClientName)
, m_midiChannel{0}
, m_midiProgram{0}
, m_asio()
, m_work()
, m_asioThread()
, m_startedNotes()
, m_started{false}
{
}

template <typename TransportT>
MusicSensorClient<TransportT>::~MusicSensorClient()
{
    stop();
}

template <typename TransportT>
void MusicSensorClient<TransportT>::start()
{
    if (!m_started)
    {
        LOG4CXX_DEBUG(logger(), "Checking MIDI ports...")
        // Check MIDI ports
        if (m_midiOut.getPortCount() < 1)
        {
            throw MidiEndpointException("No MIDI ports available");
        }

        // MIDI output
        LOG4CXX_DEBUG(logger(), "Opening MIDI port...")
        m_midiOut.openVirtualPort("midi_out");
        midiSetProgram();

        m_started = true;

        m_work.reset(new asio::io_service::work(m_asio));
        m_asioThread = std::thread([this]
                                   {
                                       m_asio.run();
                                   });

        LOG4CXX_INFO(logger(), "Music sensor client started")
    }
    else
    {
        LOG4CXX_WARN(logger(), "Music sensor client was already started")
    }
}

template <typename TransportT>
void MusicSensorClient<TransportT>::stop()
{
    if (m_started)
    {
        LOG4CXX_DEBUG(logger(), "Stopping sensor client...")
        m_started = false;
        m_work.reset();
        m_asioThread.join();
        m_midiOut.closePort();
        LOG4CXX_INFO(logger(), "Music sensor client stopped")
    }
}

template <typename TransportT>
bool MusicSensorClient<TransportT>::onDataReading(
    const TimeSpanNoteReading &reading)
{
    using namespace std::chrono;

    if (!m_started)
    {
        return true;
    }

    // Get current time stamp
    auto now = system_clock::now().time_since_epoch();
    auto nowMs = duration_cast<milliseconds>(now);

    LOG4CXX_DEBUG(logger(), "Received message:\n"
                                << reading->ShortDebugString())

    // Read message data
    milliseconds timestampMs{reading->timestamp()};
    const masmusic::Pitch &pitch = reading->pitch();
    unsigned char midiNote = pitchToMidi(pitch);
    if (reading->velocity() >= 128)
    {
        LOG4CXX_WARN(logger(), "Invalid velocity value clamped to range 0-127")
    }
    auto velocity =
        static_cast<unsigned char>(std::min(reading->velocity(), 127u));

    // Normalize expired start timestamp to now
    timestampMs = std::max(timestampMs, nowMs);
    system_clock::time_point timestampPointOn{timestampMs};
    auto timestampPointOff =
        timestampPointOn + milliseconds{reading->duration()};

    // Set up new timers
    auto onTimer =
        std::make_shared<asio::system_timer>(m_asio, timestampPointOn);
    onTimer->async_wait(
        [this, onTimer, midiNote, velocity](const asio::error_code &)
        {
            // Stop any previously playing notes
            if (m_startedNotes[midiNote].lock())
            {
                midiNoteOff(midiNote, DEFAULT_VELOCITY);
            }
            // Start note and save this timer as initiator
            m_startedNotes[midiNote] = onTimer;
            midiNoteOn(midiNote, velocity);
        });
    auto offTimer =
        std::make_shared<asio::system_timer>(m_asio, timestampPointOff);
    offTimer->async_wait(
        [this, onTimer, offTimer, midiNote](const asio::error_code &)
        {
            // Switch off note if it was initiated by this onTimer
            if (m_startedNotes[midiNote].lock() == onTimer)
            {
                midiNoteOff(midiNote, DEFAULT_VELOCITY);
            }
        });

    return true;
}

template <typename TransportT>
void MusicSensorClient<TransportT>::midiNoteOn(unsigned char midiNote,
                                               unsigned char velocity)
{
    if (!m_started)
    {
        return;
    }

    LOG4CXX_DEBUG(logger(),
                  "Note on: " << midiToPitch(midiNote).ShortDebugString())
    std::vector<unsigned char> message{
        (unsigned char)(0x90 | (0x0F & m_midiChannel)),
        (unsigned char)(midiNote & 0x7F), (unsigned char)(velocity & 0x7F)};
    m_midiOut.sendMessage(&message);
}

template <typename TransportT>
void MusicSensorClient<TransportT>::midiNoteOff(unsigned char midiNote,
                                                unsigned char velocity)
{
    if (!m_started)
    {
        return;
    }

    LOG4CXX_DEBUG(logger(),
                  "Note off: " << midiToPitch(midiNote).ShortDebugString())
    std::vector<unsigned char> message{
        (unsigned char)(0x80 | (0x0F & m_midiChannel)),
        (unsigned char)(midiNote & 0x7F), (unsigned char)(velocity & 0x7F)};
    m_midiOut.sendMessage(&message);
}

template <typename TransportT>
void MusicSensorClient<TransportT>::midiSetProgram()
{
    if (!m_started)
    {
        return;
    }

    LOG4CXX_DEBUG(logger(), "Set program: " << m_midiProgram)
    std::vector<unsigned char> message{
        (unsigned char)(0xC0 | (0x0F & m_midiChannel)), m_midiProgram};
    m_midiOut.sendMessage(&message);
}
}

#endif
