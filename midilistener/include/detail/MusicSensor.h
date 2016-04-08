
#ifndef MUSICSENSOR_DETAIL_H
#define MUSICSENSOR_DETAIL_H

#include "../MusicSensor.h"

#include <chrono>
#include <istream>

namespace midiendpoints
{

template <typename TransportT>
log4cxx::LoggerPtr
    MusicSensor<TransportT>::LOG(log4cxx::Logger::getLogger("MusicSensor"));

template <typename TransportT>
MusicSensor<TransportT>::MusicSensor(
    const TransportT &transport, const typename TransportT::Channel &channel,
    const std::string &midiClientName)
: bsf::Sensor<TransportT, TimeSpanNoteReading, TimeSpanNoteReadingFactory>(
      transport, channel)
, m_midiIn(MIDI_API, midiClientName)
, m_midiParserStatus{MidiParserStatus::WAITING_ON}
, m_reading{bsf::Sensor<TransportT, TimeSpanNoteReading,
                        TimeSpanNoteReadingFactory>::newDataReading()}
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
    double /*midiTimeStamp*/, std::vector<unsigned char> *message,
    void * /*userData*/)
{
    using namespace std::chrono;

    LOG4CXX_DEBUG(logger(), "MIDI event received (" << message->size()
                                                    << " bytes)")

    for (auto b : *message) {
        switch (m_midiParserStatus)
        {
        case MidiParserStatus::WAITING_ON:
            if ((b & 0xF0) == 0x90)
            {
                m_midiParserStatus = MidiParserStatus::WAITING_NOTE;
            }
            break;
        case MidiParserStatus::WAITING_NOTE:
            if ((b & 0x80) != 0)
            {
                if ((b & 0xF0) == 0x90)
                {
                    m_midiParserStatus = MidiParserStatus::WAITING_NOTE;
                }
                else
                {
                    m_midiParserStatus = MidiParserStatus::WAITING_ON;
                }
            }
            else
            {
                midiToPitch(b, m_reading->mutable_pitch());
                m_midiParserStatus = MidiParserStatus::WAITING_VELOCITY;
            }
            break;
        case MidiParserStatus::WAITING_VELOCITY:
            if ((b & 0x80) != 0)
            {
                LOG4CXX_WARN(logger(), "Unexpected MIDI status message")
                if ((b & 0xF0) == 0x90)
                {
                    m_midiParserStatus = MidiParserStatus::WAITING_NOTE;
                }
                else
                {
                    m_midiParserStatus = MidiParserStatus::WAITING_ON;
                }
            }
            else
            {
                // Get time stamp
                auto timeStamp = system_clock::now().time_since_epoch();
                auto timeStampMs =
                    duration_cast<milliseconds>(timeStamp).count();

                m_reading->set_velocity(b);
                m_reading->set_timestamp(timeStampMs);
                LOG4CXX_DEBUG(logger(), "Publishing message:\n"
                                            << m_reading->ShortDebugString())
                bsf::Sensor<TransportT, TimeSpanNoteReading,
                            TimeSpanNoteReadingFactory>::publish(m_reading);

                m_midiParserStatus = MidiParserStatus::WAITING_NOTE;
            }
            break;
        }
    }
}

template <typename TransportT>
void MusicSensor<TransportT>::forwardMidiCallback(
    double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    auto retransmitter = static_cast<MusicSensor<TransportT> *>(userData);
    if (retransmitter)
    {
        retransmitter->midiEventReceived(timeStamp, message, userData);
    }
    else
    {
        LOG4CXX_ERROR(LOG, "Invalid pointer in MIDI event callback")
    }
}
}

#endif
