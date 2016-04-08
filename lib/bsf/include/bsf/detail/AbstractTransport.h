
#ifndef BSF_ABSTRACTTRANSPORT_DETAIL_H
#define BSF_ABSTRACTTRANSPORT_DETAIL_H

#include "../AbstractTransport.h"

namespace bsf
{

//! \brief Implementation
template <typename ChannelT>
class AbstractTransport<ChannelT>::Impl
{
public:
    explicit Impl(AbstractTransport<Channel> *obj)
    : m_obj(obj)
    , m_handlers()
    , m_currentToken{0}
    {
    }

    virtual ~Impl()
    {
    }

    HandlerToken addHandler(Handler handler,
                            const Channel &channel = Channel())
    {
        HandlerToken assignedToken = m_currentToken;
        auto channelEntry = m_handlers.find(channel);
        if (channelEntry == m_handlers.end())
        {
            m_handlers[channel] = {{assignedToken, std::move(handler)}};
            m_obj->useChannel(channel);
        }
        else
        {
            channelEntry->second[assignedToken] = std::move(handler);
        }
        m_currentToken++;
        return assignedToken;
    }

    void removeHandler(const HandlerToken token,
                       const Channel &channel = Channel())
    {
        auto channelEntry = m_handlers.find(channel);
        if (channelEntry == m_handlers.end())
        {
            return;
        }
        auto &channelHandlers = channelEntry->second;
        channelHandlers.erase(token);
        if (channelHandlers.empty())
        {
            m_handlers.erase(channelEntry);
            m_obj->dropChannel(channel);
        }
    }

    void callHandlers(const std::vector<unsigned char> &message,
                      const Channel &channel = Channel()) const
    {
        const auto &channelEntry = m_handlers.find(channel);
        if (channelEntry == m_handlers.end())
        {
            return;
        }
        for (const auto &handlerEntry : channelEntry->second) {
            (handlerEntry.second)(message);
        }
    }

    std::vector<Channel> getChannelsInUse() const
    {
        std::vector<Channel> channels;
        channels.reserve(m_handlers.size());
        for (const auto &channelEntry : m_handlers) {
            channels.push_back(channelEntry.first);
        }
        return channels;
    }

private:
    typedef std::unordered_map<HandlerToken, Handler> Handlers;

    AbstractTransport<Channel> *m_obj;
    std::unordered_map<Channel, Handlers> m_handlers;
    HandlerToken m_currentToken;
};

template <typename ChannelT>
AbstractTransport<ChannelT>::AbstractTransport()
: m_impl(std::make_shared<AbstractTransport<ChannelT>::Impl>(this))
{
}

template <typename ChannelT>
AbstractTransport<ChannelT>::~AbstractTransport()
{
}

template <typename ChannelT>
HandlerToken AbstractTransport<ChannelT>::addHandler(
    typename AbstractTransport<ChannelT>::Handler handler,
    const ChannelT &channel)
{
    return m_impl->addHandler(std::move(handler), channel);
}

template <typename ChannelT>
void AbstractTransport<ChannelT>::removeHandler(const HandlerToken token,
                                                const ChannelT &channel)
{
    m_impl->removeHandler(token, channel);
}

template <typename ChannelT>
void AbstractTransport<ChannelT>::callHandlers(
    const std::vector<unsigned char> &message, const Channel &channel) const
{
    m_impl->callHandlers(message, channel);
}

template <typename ChannelT>
std::vector<ChannelT> AbstractTransport<ChannelT>::getChannelsInUse() const
{
    return m_impl->getChannelsInUse();
}

} // bsf

#endif
