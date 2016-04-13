
#ifndef BSF_ASYNCMQTTTRANSPORT_DETAIL_H
#define BSF_ASYNCMQTTTRANSPORT_DETAIL_H

#include "../AsyncMqttTransport.h"

#include <mosquittopp.h>

namespace bsf
{

namespace detail
{

//! \brief Mosquitto initalization and cleanup manager.
class MqttInitializer
{
public:
    MqttInitializer(const MqttInitializer &) = delete;
    MqttInitializer &operator=(const MqttInitializer &) = delete;

private:
    friend class WeakSingleton<MqttInitializer>;

    MqttInitializer()
    {
        mosqpp::lib_init();
    }

    virtual ~MqttInitializer()
    {
        mosqpp::lib_cleanup();
    }
};

}

//! \brief Implementation.
struct AsyncMqttTransport::Impl : public mosqpp::mosquittopp
{
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;

    Impl(AsyncMqttTransport *obj, const std::string &clientName,
         const std::string &server, unsigned int port, int qos)
    : mosqpp::mosquittopp(clientName.c_str())
    , m_obj{obj}
    , m_started{false}
    , m_server{server}
    , m_port{port}
    , m_qos{qos}
    , m_retain{false}
    , m_init(WeakSingleton<detail::MqttInitializer>::getInstance())
    {
    }

    virtual ~Impl()
    {
        stop();
    }

    void publish(const std::vector<unsigned char> &message,
                 const std::string &channel)
    {
        if (m_started)
        {
            mosqpp::mosquittopp::publish(nullptr, channel.c_str(),
                                         message.size(), message.data(), m_qos,
                                         m_retain);
        }
    }

    void start()
    {
        if (!m_started)
        {
            connect(m_server.c_str(), m_port);
            loop_start();
            for (const auto &channel : m_obj->getChannelsInUse()) {
                subscribe(nullptr, channel.c_str(), m_qos);
            }
            m_started = true;
        }
    }

    void stop()
    {
        if (m_started)
        {
            for (const auto &channel : m_obj->getChannelsInUse()) {
                unsubscribe(nullptr, channel.c_str());
            }
            disconnect();
            loop_stop();
            m_started = false;
        }
    }

    void useChannel(const std::string &channel)
    {
        if (m_started)
        {
            subscribe(nullptr, channel.c_str(), m_qos);
        }
    }

    void dropChannel(const std::string &channel)
    {
        if (m_started)
        {
            unsubscribe(nullptr, channel.c_str());
        }
    }

    virtual void on_message(const struct mosquitto_message *message)
    {
        auto payload = reinterpret_cast<unsigned char *>(message->payload);
        std::vector<unsigned char> data(payload, payload + message->payloadlen);
        m_obj->callHandlers(std::move(data), message->topic);
    }

    AsyncMqttTransport *const m_obj;
    bool m_started;
    std::string m_server;
    unsigned int m_port;
    int m_qos;
    bool m_retain;
    std::shared_ptr<detail::MqttInitializer> m_init;
};

AsyncMqttTransport::AsyncMqttTransport(const std::string &clientName,
                                       const std::string &server,
                                       unsigned int port, int qos)
: AbstractTransport<AsyncMqttTransport::Channel>()
, m_impl(std::make_shared<AsyncMqttTransport::Impl>(this, clientName, server,
                                                    port, qos))
{
}

AsyncMqttTransport::~AsyncMqttTransport()
{
}

void AsyncMqttTransport::publish(const std::vector<unsigned char> &message,
                                 const AsyncMqttTransport::Channel &channel)
{
    m_impl->publish(message, channel);
}

void AsyncMqttTransport::start()
{
    m_impl->start();
}

void AsyncMqttTransport::stop()
{
    m_impl->stop();
}

int AsyncMqttTransport::getQos() const
{
    return m_impl->m_qos;
}

void AsyncMqttTransport::setQos(int qos)
{
    m_impl->m_qos = qos;
}

bool AsyncMqttTransport::isRetained() const
{
    return m_impl->m_retain;
}

void AsyncMqttTransport::setRetained(bool retain)
{
    m_impl->m_retain = retain;
}

void AsyncMqttTransport::useChannel(const AsyncMqttTransport::Channel &channel)
{
    m_impl->useChannel(channel);
}

void AsyncMqttTransport::dropChannel(const AsyncMqttTransport::Channel &channel)
{
    m_impl->dropChannel(channel);
}

} // bsf

#endif
