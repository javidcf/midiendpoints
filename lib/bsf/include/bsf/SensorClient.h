
#ifndef BSF_SENSORCLIENT_H
#define BSF_SENSORCLIENT_H

#include "common.h"

#include <functional>
#include <unordered_map>
#include <utility>

namespace bsf
{

//!
//! \brief A client for the data readings published into a network.
//!
//! This class can be instanced or inherited to obtain data readings published
//! by sensors into a BSF network.
//!
//! \tparam TransportT Transport type for the sensor client
//! \tparam DataReadingT Data reading type for the sensor client
//! \tparam SerializerT Serializer type for the sensor client
//! \tparam DataReadingFactoryT Data reading factory type for the sensor client
//!
template <typename TransportT, typename DataReadingT,
          typename SerializerT = DefaultSerializer<DataReadingT>,
          typename DataReadingFactoryT =
              DefaultDataReadingFactory<DataReadingT>>
class SensorClient
{
public:
    //! Transport type for the sensor client.
    typedef TransportT Transport;
    //! Channel type for the for the sensor client transport.
    typedef typename TransportT::Channel Channel;
    //! Data reading type for the sensor client.
    typedef DataReadingT DataReading;
    //! Serializer type for the sensor.
    typedef SerializerT Serializer;
    //! Data reading type for the sensor.
    typedef DataReadingFactoryT DataReadingFactory;
    //! Handler function for received data readings.
    typedef std::function<void(const DataReading &)> Handler;

    SensorClient(const SensorClient &) = delete;
    SensorClient &operator=(const SensorClient &) = delete;

    //!
    //! \brief Constructor.
    //!
    //! \param transport Transport channel to be used by the sensor client
    //! \param channel Transport channel from where the readings are received
    //! \param factory Data reading factory
    //!
    SensorClient(Transport transport, Channel channel = Channel(),
                 Serializer serializer = Serializer(),
                 DataReadingFactory factory = DataReadingFactory())
    : m_transport{std::move(transport)}
    , m_channel{std::move(channel)}
    , m_serializer{std::move(serializer)}
    , m_factory{std::move(factory)}
    , m_handlers()
    , m_currentToken{0}
    , m_transportToken(m_transport.addHandler(
          std::bind(&SensorClient<Transport, DataReading, Serializer,
                                  DataReadingFactory>::processMessage,
                    this, std::placeholders::_1),
          m_channel))
    {
    }

    //!
    //! \brief Destructor.
    //!
    virtual ~SensorClient()
    {
        m_transport.removeHandler(m_transportToken);
    }

    //!
    //! \brief Get the transport of the sensor client.
    //!
    //! \return The transport of the sensor client
    //!
    const Transport &getTransport() const
    {
        return m_transport;
    }

    //!
    //! \brief Get the transport of the sensor client.
    //!
    //! \return The transport of the sensor client
    //!
    Transport &getTransport()
    {
        return m_transport;
    }

    //!
    //! \brief Get the channel of the sensor client.
    //!
    //! \return The channel of the sensor client
    //!
    const Channel &getChannel() const
    {
        return m_channel;
    }

    //!
    //! \brief Add a new handler for new incoming data readings.
    //!
    //! If the function already exists this function has no effect.
    //!
    //! Each handler is called once for every new received data reading.
    //! Handlers are called in unspecific order, and, depending on the
    //! transport, they may be called from different threads.
    //!
    //! This function is not thread-safe.
    //!
    //! \param handler New handler
    //! \return A token of the registration to be used on handler removal
    //!
    HandlerToken addHandler(Handler handler)
    {
        HandlerToken assignedToken = m_currentToken;
        m_handlers[assignedToken] = std::move(handler);
        m_currentToken++;
        return assignedToken;
    }

    //!
    //! \brief Remove a handler from the sensor client.
    //!
    //! If the handler does not exist this function has no effect.
    //!
    //! This function is not thread-safe.
    //!
    //! \param token Token of the handler to remove
    //!
    void removeHandler(const HandlerToken token)
    {
        m_handlers.erase(token);
    }

protected:
    //!
    //! \brief New data reading callback.
    //!
    //! This method can be redefined by subclasses to process data readings
    //! without needing to register a handler. This method gets called before
    //! any registered handler, and its return value can be used to determine
    //! whether the reading should be processed by the handlers.
    //!
    //! \param reading New data reading
    //! \return Whether the reading should be processed by the handlers
    //!
    virtual bool onDataReading(const DataReading & /*reading*/)
    {
        return true;
    }

private:
    //! Transport for the sensor client
    Transport m_transport;
    //! Channel for the sensor client
    const Channel m_channel;
    //! Message serializer
    Serializer m_serializer;
    //! Data reading factory
    DataReadingFactory m_factory;
    //! Handlers for the received readings
    std::unordered_map<HandlerToken, Handler> m_handlers;
    //! Next handler token to be assigned
    HandlerToken m_currentToken;
    //! Token of the registered callback in the transport
    const HandlerToken m_transportToken;

    //!
    //! \brief Process a received message.
    //!
    //! Makes a data reading out of the received message and calls every
    //! handler.
    //!
    //! \param message Message data
    //!
    void processMessage(const std::vector<unsigned char> message)
    {
        try
        {
            DataReading reading{m_factory.newDataReading()};
            m_serializer.deserialize(message, reading);
            auto runHandlers = onDataReading(reading);
            if (runHandlers)
            {
                for (const auto &handler : m_handlers) {
                    (handler.second)(reading);
                }
            }
        }
        catch (const SerializationError &)
        {
            // TODO raise or ignore?
        }
    }
};

} // bsf

#endif
