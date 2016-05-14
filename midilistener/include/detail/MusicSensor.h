
#ifndef MUSICSENSOR_DETAIL_H
#define MUSICSENSOR_DETAIL_H

#include "../MusicSensor.h"

#include <istream>

namespace midiendpoints
{

template <typename TransportT>
log4cxx::LoggerPtr
    MusicSensor<TransportT>::LOG(log4cxx::Logger::getLogger("MusicSensor"));

template <typename TransportT>
MusicSensor<TransportT>::MusicSensor(
    const TransportT &transport,
    const typename TransportT::Channel &channelInstant,
    const typename TransportT::Channel &channelSpanned,
    const std::string &midiClientName)
: m_sensorInstant(transport, channelInstant)
, m_sensorSpanned(transport, channelSpanned)
, m_midiIn(MIDI_API, midiClientName)
, m_midiNoteEvent(MidiNoteEvent::NONE)
, m_midiParserStatus{MidiParserStatus::WAITING}
, m_readingInstant{m_sensorInstant.newDataReading()}
, m_readingSpanned{m_sensorSpanned.newDataReading()}
, m_currentPitch{0}
, m_startedNotes()
, m_started{false}
{
}

template <typename TransportT>
MusicSensor<TransportT>::~MusicSensor()
{
    stop();
}

template <typename TransportT>
void MusicSensor<TransportT>::start()
{
    if (!m_started)
    {
        // Check MIDI ports
        if (m_midiIn.getPortCount() < 1)
        {
            throw MidiEndpointException("No MIDI ports available");
        }

        // MIDI input
        m_midiIn.openPort(0, "midi_in");
        m_midiIn.setCallback(MusicSensor<TransportT>::forwardMidiCallback,
                             this);
        // midiin->ignoreTypes( false, false, false );

        m_started = true;

        LOG4CXX_INFO(logger(), "Music sensor started")
    }
    else
    {
        LOG4CXX_WARN(logger(), "Music sensor was already started")
    }
}

template <typename TransportT>
void MusicSensor<TransportT>::stop()
{
    if (m_started)
    {
        LOG4CXX_DEBUG(logger(), "Stopping sensor...")
        m_started = false;
        m_midiIn.cancelCallback();
        m_midiIn.closePort();
        LOG4CXX_INFO(logger(), "Music sensor stopped")
    }
}

template <typename TransportT>
void MusicSensor<TransportT>::midiEventReceived(
    double /*midiTimestamp*/, std::vector<unsigned char> *message,
    void * /*userData*/)
{
    using namespace std::chrono;

    LOG4CXX_DEBUG(logger(), "MIDI event received (" << message->size()
                                                    << " bytes)")

    for (auto messageByte : *message) {
        // All this would probably be better done with a proper FSM
        if ((messageByte & 0x80) != 0)
        {
            // Status byte
            if ((messageByte & 0xF0) == 0x80)
            {
                m_midiNoteEvent = MidiNoteEvent::OFF;
                m_midiParserStatus = MidiParserStatus::NOTE;
            }
            else if ((messageByte & 0xF0) == 0x90)
            {
                m_midiNoteEvent = MidiNoteEvent::ON;
                m_midiParserStatus = MidiParserStatus::NOTE;
            }
            else
            {
                m_midiNoteEvent = MidiNoteEvent::NONE;
                m_midiParserStatus = MidiParserStatus::WAITING;
            }
        }
        else if (m_midiNoteEvent != MidiNoteEvent::NONE)
        {
            // Data byte
            switch (m_midiParserStatus)
            {
            case MidiParserStatus::NOTE:
            {
                m_currentPitch = messageByte;
                m_midiParserStatus = MidiParserStatus::VELOCITY;
                break;
            }
            case MidiParserStatus::VELOCITY:
            {
                // Get time stamp
                auto timestamp = system_clock::now();
                auto timestampMs = duration_cast<milliseconds>(
                                       timestamp.time_since_epoch()).count();

                // Send spanned event on OFF or repeated ON
                auto previous = m_startedNotes.find(m_currentPitch);
                if ((m_midiNoteEvent == MidiNoteEvent::OFF) ||
                    (previous != m_startedNotes.end()))
                {
                    // Compute note timestamp and duration in milliseconds
                    auto previousTimestamp = previous->second.timestamp;
                    auto previousMs =
                        duration_cast<milliseconds>(
                            previousTimestamp.time_since_epoch()).count();
                    auto durationMs =
                        static_cast<unsigned int>(timestampMs - previousMs);
                    // Fill spanned reading data
                    auto previousVelocity = previous->second.velocity;
                    midiToPitch(m_currentPitch,
                                m_readingSpanned->mutable_pitch());
                    m_readingSpanned->set_timestamp(previousMs);
                    m_readingSpanned->set_velocity(previousVelocity);
                    m_readingSpanned->set_duration(durationMs);
                    // Remove previous onset
                    m_startedNotes.erase(previous);
                    // Publish message
                    LOG4CXX_DEBUG(logger(),
                                  "Publishing spanned message:\n"
                                      << m_readingSpanned->ShortDebugString())
                    m_sensorSpanned.publish(m_readingSpanned);
                }

                if (m_midiNoteEvent == MidiNoteEvent::ON)
                {
                    // Fill instant reading data
                    midiToPitch(m_currentPitch,
                                m_readingInstant->mutable_pitch());
                    m_readingInstant->set_timestamp(timestampMs);
                    m_readingInstant->set_velocity(messageByte);
                    // Save onset data
                    m_startedNotes[m_currentPitch] = {timestamp, messageByte};
                    // Publish message
                    LOG4CXX_DEBUG(logger(),
                                  "Publishing instant message:\n"
                                      << m_readingInstant->ShortDebugString())
                    m_sensorInstant.publish(m_readingInstant);
                }

                m_midiParserStatus = MidiParserStatus::NOTE;
                break;
            }
            default:
                LOG4CXX_WARN(LOG, "Inconsistent MIDI parser status")
            }
        }
    }
}

template <typename TransportT>
void MusicSensor<TransportT>::forwardMidiCallback(
    double timestamp, std::vector<unsigned char> *message, void *userData)
{
    auto retransmitter = static_cast<MusicSensor<TransportT> *>(userData);
    if (retransmitter)
    {
        retransmitter->midiEventReceived(timestamp, message, userData);
    }
    else
    {
        LOG4CXX_ERROR(LOG, "Invalid pointer in MIDI event callback")
    }
}
}

#endif
