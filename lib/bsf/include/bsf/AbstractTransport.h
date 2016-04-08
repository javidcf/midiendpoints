
#ifndef BSF_ABSTRACTTRANSPORT_H
#define BSF_ABSTRACTTRANSPORT_H

#include "common.h"

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace bsf
{

//!
//! \brief Base class for transport implementations.
//!
//! This class provides common declarations for classes implementing a
//! transport. While transport implementations are not strictly required to
//! subclass this template, it is generally convenient to do so.
//!
//! \tparam ChannelT Channel type for the transport
//!
template <typename ChannelT>
class AbstractTransport
{
public:
    // Channel type for the transport.
    typedef ChannelT Channel;

    //! Handler type for the transport
    typedef std::function<void(const std::vector<unsigned char> &)> Handler;

    //!
    //! \brief Constructor.
    //!
    AbstractTransport();

    //!
    //! \brief Destructor.
    //!
    virtual ~AbstractTransport();

    //!
    //! \brief Add a new handler for new incoming messages in a channel.
    //!
    //! If the handler already exists it will not be added twice.
    //!
    //! Each handler is called once for every new received message in the
    //! channel. Handlers are called in unspecific order, and, depending on the
    //! transport, they may be called from different threads.
    //!
    //! This function is not thread-safe.
    //!
    //! \param handler New handler
    //! \param channel Channel where the handler is added
    //!
    HandlerToken addHandler(Handler handler,
                            const Channel &channel = Channel());

    //!
    //! \brief Remove a handler for a channel in the transport.
    //!
    //! If the handler does not exist this function has no effect.
    //!
    //! This function is not thread-safe.
    //!
    //! \param token Token of the handler to remove
    //! \param channel Channel from where the handler is removed
    //!
    void removeHandler(const HandlerToken token,
                       const Channel &channel = Channel());

    //!
    //! \brief Publish a message to the transport.
    //!
    //! \param message Message to publish.
    //! \param channel Channel where the reading will be published.
    //!
    virtual void publish(const std::vector<unsigned char> &message,
                         const Channel &channel = Channel()) = 0;

protected:
    //!
    //! \brief Call every handler of a channel with a message.
    //!
    //! \param message Message data
    //! \param channel Channel where the message was received
    //!
    void callHandlers(const std::vector<unsigned char> &message,
                      const Channel &channel = Channel()) const;

    //!
    //! \brief Get the channels that are currently being used.
    //!
    //! A channel is considered to be in use when there is at least one handler
    //! attached to it.
    //!
    //! \return The channels that are in use
    //!
    std::vector<Channel> getChannelsInUse() const;

    //!
    //! \brief Notifies that a new channel is being used.
    //!
    //! This method is called every time a handler is added to a channel with no
    //! other handler assigned previously. The default implementation does
    //! nothing, but subclasses may redefine it.
    //!
    //! \brief channel New channel
    //!
    virtual void useChannel(const Channel & /*channel*/)
    {
    }

    //!
    //! \brief Notifies that a channel is not being used anymore.
    //!
    //! This method is called every time a handler is removed from a channel
    //! with no more handlers. The default implementation does nothing, but
    //! subclasses may redefine it.
    //!
    //! \brief channel Drop channel
    //!
    virtual void dropChannel(const Channel & /*channel*/)
    {
    }

private:
    //! Pimpl
    class Impl;
    friend class Impl;
    std::shared_ptr<Impl> m_impl;
};

} // bsf

#include "detail/AbstractTransport.h"

#endif
