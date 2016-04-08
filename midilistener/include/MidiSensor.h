
#ifndef MIDISENSOR_H
#define MIDISENSOR_H

#include <b64/encode.h>
#include <bsf/Sensor.h>
#include <bsf/JsonDataReading.h>
#include <log4cxx/logger.h>
#include <RtMidi.h>

#include <string>
#include <sstream>
#include <vector>

namespace midiendpoints
{

//!
//! \brief Retransmits MIDI messages to a BSF network.
//!
//! MIDI messages are received through a Jack MIDI port and retransmitted into a
//! BSF network in JSON format.
//!
//! \tparam TransportT BSF transport type
//!
template <typename TransportT>
class MidiSensor : private bsf::Sensor<TransportT, bsf::JsonDataReading>
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
    MidiSensor(const Transport &transport,
               const typename Transport::Channel &channel,
               const std::string &midiClientName);

    //!
    //! \brief Start the sensor.
    //!
    //! Opens a MIDI port and start publishing the events to the network.
    //!
    void start();

    //!
    //! \brief Stop the sensor.
    //!
    //! Closes any open MIDI and stops publishing events to the network.
    //!
    void stop();

    //!
    //! \brief Destructor.
    //!
    virtual ~MidiSensor();

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
    //! MIDI input
    RtMidiIn m_midiIn;
    //! Midi client name
    std::string m_midiClientName;
    //! Base64 encoder
    base64::encoder m_b64Encoder;
    //! String stream for MIDI message data
    std::ostringstream m_encodedData;
    //! Whether the retransmitter has been started
    bool m_started;

    //! Class logger
    static log4cxx::LoggerPtr LOG;
    //! \return Class logger.
    constexpr log4cxx::LoggerPtr logger() const
    {
        return MidiSensor<Transport>::LOG;
    }

    //!
    //! \brief Forward a MIDI event callback to a MidiSensor object.
    //!
    //! This is a wrapper for the member callback function, since RtMidi does
    // not
    //! allow to use bound functions as callback.
    //!
    //! \param timeStamp time stamp of the MIDI event
    //! \param message MIDI event data
    //! \param userData a valid pointer to a MidiSensor
    //!
    static void forwardMidiCallback(double timeStamp,
                                    std::vector<unsigned char> *message,
                                    void *userData);
};
}

#include "detail/MidiSensor.h"

#endif
