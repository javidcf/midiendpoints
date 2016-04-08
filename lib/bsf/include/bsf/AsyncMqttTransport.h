
#ifndef BSF_ASYNCMQTTTRANSPORT_H
#define BSF_ASYNCMQTTTRANSPORT_H

#include "AbstractTransport.h"

#include <functional>
#include <memory>
#include <string>

namespace bsf
{

//!
//! \brief Asynchronous MQTT transport.
//!
//! MQTT transport for a BSF network. This is an asynchronous transport;
//! interaction with the server is performed asynchronously by managed threads.
//!
//! Instancing this class does not start the operation of the transport. It is
//! necessary to call \link AsyncMqttTransport::start for the transport to
//! interact with the server.
//!
//! \todo Manage connection failures and disconnections.
//!
class AsyncMqttTransport : public AbstractTransport<std::string>
{
public:
    //!
    //! \brief Constructor.
    //!
    //! \param clientName Client identifier
    //! \param server MQTT server name or address
    //! \param port MQTT server port
    //! \param qos MQTT QoS (0, 1 or 2)
    //!
    AsyncMqttTransport(const std::string &clientName, const std::string &server,
                       unsigned int port, int qos);

    //!
    //! \brief Destructor.
    //!
    virtual ~AsyncMqttTransport();

    //!
    //! \copydoc AbstractTransport::publish
    //!
    void publish(const std::vector<unsigned char> &message,
                 const Channel &channel = Channel());

    //!
    //! \brief Start the MQTT transport loop.
    //!
    //! Starts the operation of the MQTT transport.
    //!
    void start();

    //!
    //! \brief Stop the MQTT transport loop.
    //!
    //! Stops the operation of the MQTT transport.
    //!
    void stop();

    //!
    //! \brief Get the configured MQTT quality of service.
    //!
    //! \return The configured MQTT quality of service
    //!
    int getQos() const;

    //!
    //! \brief Set the MQTT quality of service.
    //!
    //! Changing the quality of service will only have effect on future
    //! publications and channel subscriptions.
    //!
    //! \param qos The new MQTT quality of service
    //!
    void setQos(int qos);

    //!
    //! \brief Get the configured MQTT retainment policy.
    //!
    //! \return true if the messages should be retained on the server, false
    //!         otherwise
    //!
    bool isRetained() const;

    //!
    //! \brief Set the MQTT retainment policy.
    //!
    //! \param retain true if the messages should be retained on the server,
    //!               false otherwise
    //!
    void setRetained(bool retain);

protected:
    //!
    //! \brief Subscribe to a channel.
    //!
    //! \brief channel New channel
    //!
    virtual void useChannel(const Channel &channel);

    //!
    //! \brief Unsubscribe from the channel.
    //!
    //! \brief channel Drop channel
    //!
    virtual void dropChannel(const Channel &channel);

private:
    //! Pimpl
    class Impl;
    friend class Impl;
    std::shared_ptr<Impl> m_impl;
};

} // bsf

#include "detail/AsyncMqttTransport.h"

#endif
