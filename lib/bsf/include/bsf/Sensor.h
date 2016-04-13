
#ifndef BSF_SENSOR_H
#define BSF_SENSOR_H

#include "common.h"

#include <utility>

namespace bsf
{

//!
//! \brief Sensing entity that publishes data readings into a network.
//!
//! This class can be instanced or inherited to publish data readings obtained
//! through some input (e.g. a hardware sensor or some user input) into the BSF
//! network.
//!
//! \tparam TransportT Transport type for the sensor
//! \tparam DataReadingT Data reading type for the sensor
//! \tparam SerializerT Serializer type for the sensor
//! \tparam DataReadingFactoryT Data reading factory type for the sensor
//!
template <typename TransportT, typename DataReadingT,
          typename SerializerT = DefaultSerializer<DataReadingT>,
          typename DataReadingFactoryT =
              DefaultDataReadingFactory<DataReadingT>>
class Sensor
{
public:
    //! Transport type for the sensor.
    typedef TransportT Transport;
    //! Channel type for the for the sensor transport.
    typedef typename TransportT::Channel Channel;
    //! Data reading type for the sensor.
    typedef DataReadingT DataReading;
    //! Serializer type for the sensor.
    typedef SerializerT Serializer;
    //! Data reading type for the sensor.
    typedef DataReadingFactoryT DataReadingFactory;

    Sensor(const Sensor &) = delete;
    Sensor &operator=(const Sensor &) = delete;

    //!
    //! \brief Constructor.
    //!
    //! \param transport Transport to be used by the sensor
    //! \param channel Transport channel where the readings will be published
    //! \param serializer Data reading serializer
    //! \param factory Data reading factory
    //!
    Sensor(Transport transport, Channel channel = Channel(),
           Serializer serializer = Serializer(),
           DataReadingFactory factory = DataReadingFactory())
    : m_transport{std::move(transport)}
    , m_channel{std::move(channel)}
    , m_serializer{std::move(serializer)}
    , m_factory{std::move(factory)}
    {
    }

    //!
    //! \brief Destructor.
    //!
    virtual ~Sensor()
    {
    }

    //!
    //! \brief Get the transport of the sensor.
    //!
    //! \return The transport of the sensor
    //!
    const Transport &getTransport() const
    {
        return m_transport;
    }

    //!
    //! \brief Get the transport of the sensor.
    //!
    //! \return The transport of the sensor
    //!
    Transport &getTransport()
    {
        return m_transport;
    }

    //!
    //! \brief Get the channel of the sensor.
    //!
    //! \return The channel of the sensor
    //!
    const Channel &getChannel() const
    {
        return m_channel;
    }

    //!
    //! \brief Create a new data reading.
    //!
    //! You should use this method to create new data readings instead of
    //! creating them on your own, since it ensures that the configure data
    //! reading factory is being used.
    //!
    //! \return A new data reading
    //!
    DataReading newDataReading()
    {
        return m_factory.newDataReading();
    }

    //!
    //! \brief Publish a data reading to the communication channel.
    //!
    //! \param reading The data reading to publish
    //!
    void publish(const DataReading &reading)
    {
        try
        {
            std::vector<unsigned char> message;
            m_serializer.serialize(reading, message);
            m_transport.publish(message, m_channel);
        }
        catch (const SerializationError &)
        {
            // TODO raise or ignore?
        }
    }

private:
    //! Transport for the sensor
    Transport m_transport;
    //! Channel for the sensor
    const Channel m_channel;
    //! Message serializer
    Serializer m_serializer;
    //! Data reading factory
    DataReadingFactory m_factory;
};

} // bsf

#endif
