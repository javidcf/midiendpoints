
#ifndef BSF_PROTOBUFPTRDATAREADING_H
#define BSF_PROTOBUFPTRDATAREADING_H

#include "common.h"

#include <google/protobuf/arena.h>

#include <memory>
#include <utility>
#include <vector>

namespace bsf
{

//!
//! \brief Data reading encapsulating a protocol buffers message pointer.
//!
//! This class does not manage the pointed protocl buffers message (i.e. it does
//! not destroy the object). It is intended to be used with arena allocation.
//!
//! \tparam MessageT Protocol buffers message type
//!
template <typename MessageT>
class ProtobufPtrDataReading
{
public:
    //!
    //! \brief Constructor.
    //!
    //! Creates a data reading with an null message pointer.
    //!
    ProtobufPtrDataReading()
    : m_message(nullptr)
    {
    }

    //!
    //! \brief Constructor.
    //!
    //! \param message Protocol buffers message pointer to encapsulate
    //!
    ProtobufPtrDataReading(MessageT *message)
    : m_message{message}
    {
    }

    //! \return The encapsulated message pointer
    const MessageT *operator->() const
    {
        return m_message;
    }

    //! \return The encapsulated message pointer
    MessageT *operator->()
    {
        return m_message;
    }

    //! \return The encapsulated message
    const MessageT &operator*() const
    {
        return *m_message;
    }

    //! \return The encapsulated message
    MessageT &operator*()
    {
        return *m_message;
    }

    //! \param message The new encapsulated message pointer
    void setMessage(MessageT *message)
    {
        m_message = message;
    }

private:
    //! The encapsulated message pointer
    MessageT *m_message;
};

template <>
template <typename MessageT>
class DefaultSerializer<ProtobufDataReading<MessageT>>
    : public AbstractSerializer<ProtobufDataReading<MessageT>>
{
public:
    void serialize(const ProtobufPtrDataReading<MessageT> &reading,
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
                     ProtobufPtrDataReading<MessageT> &reading) const
    {
        auto ok = reading->ParseFromArray(message.data(), message.size());
        if (!ok)
        {
            throw SerializationError(
                "Could not parse protocol buffers message");
        }
    }
};

//!
//! \brief An arena-based protocol buffers data reading factory.
//!
template <typename MessageT>
class ProtobufArenaReadingFactory
    : public AbstractDataReadingFactory<ProtobufPtrDataReading<MessageT>>
{
public:
    //!
    //! \brief Constructor.
    //!
    //! Creates a data reading factory using the default singleton arena.
    //!
    ProtobufArenaReadingFactory()
    : DefaultDataReadingFactory<ProtobufPtrDataReading<MessageT>>()
    , m_arena{WeakSingleton<Arena>::getInstance()}
    {
    }

    //!
    //! \brief Constructor.
    //!
    //! \param arena Protocol buffers arena
    //!
    ProtobufArenaReadingFactory(std::shared_ptr<Arena> arena)
    : DefaultDataReadingFactory<ProtobufPtrDataReading<MessageT>>()
    , m_arena{std::move{arena}}
    {
    }

    ProtobufPtrDataReading<MessageT> newDataReading() const
    {
        return ProtobufPtrDataReading<MessageT>(
            Arena::CreateMessage<MessageT>(m_arena.get()));
    }

private:
    typedef google::protobuf::Arena Arena;
    typedef google::protobuf::Arena Arena;

    //! Protocol buffers arena
    std::shared_ptr<Arena> m_arena;
};

} // bsf

#endif
