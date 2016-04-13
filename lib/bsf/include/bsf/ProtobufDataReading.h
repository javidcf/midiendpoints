
#ifndef BSF_PROTOBUFDATAREADING_H
#define BSF_PROTOBUFDATAREADING_H

#include "common.h"

#include <utility>
#include <vector>

namespace bsf
{

//!
//! \brief Data reading encapsulating a protocol buffers message.
//!
//! \tparam MessageT Protocol buffers message type
//!
template <typename MessageT>
class ProtobufDataReading
{
public:
    //!
    //! \brief Constructor.
    //!
    //! Creates a data reading with an empty message.
    //!
    ProtobufDataReading()
    : m_message()
    {
    }

    //!
    //! \brief Constructor.
    //!
    //! \param message Protocol buffers message to encapsulate
    //!
    ProtobufDataReading(MessageT message)
    : m_message{std::move(message)}
    {
    }

    //! \return The encapsulated message
    const MessageT *operator->() const
    {
        return &m_message;
    }

    //! \return The encapsulated message
    MessageT *operator->()
    {
        return &m_message;
    }

    //! \return The encapsulated message
    const MessageT &operator*() const
    {
        return m_message;
    }

    //! \return The encapsulated message
    MessageT &operator*()
    {
        return m_message;
    }

    //! \param message The new encapsulated message
    void setMessage(MessageT message)
    {
        m_message = std::move(message);
    }

private:
    //! The encapsulated message
    MessageT m_message;
};

//!
//! \brief Default serializer for protocol buffers pointer data readings.
//!
template <>
template <typename MessageT>
class DefaultSerializer<ProtobufDataReading<MessageT>>
    : public AbstractSerializer<ProtobufDataReading<MessageT>>
{
public:
    void serialize(const ProtobufDataReading<MessageT> &reading,
                   std::vector<unsigned char> &message) const
    {
        message.resize(reading->ByteSize());
        auto ok = reading->SerializeToArray(message.data(), message.size());
        if (!ok)
        {
            message.clear();
            throw SerializationError(
                "Could not serialize protocol buffers message");
        }
    }

    void deserialize(const std::vector<unsigned char> &message,
                     ProtobufDataReading<MessageT> &reading) const
    {
        auto ok = reading->ParseFromArray(message.data(), message.size());
        if (!ok)
        {
            throw SerializationError(
                "Could not parse protocol buffers message");
        }
    }
};

} // bsf

#endif
