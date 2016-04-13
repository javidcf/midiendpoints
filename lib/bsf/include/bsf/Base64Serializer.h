
#ifndef BSF_BASE64SERIALIZER_H
#define BSF_BASE64SERIALIZER_H

#include "common.h"

#include <b64/encode.h>
#include <b64/decode.h>

#include <sstream>
#include <vector>

namespace bsf
{

//!
//! \brief Base64 serializer.
//!
//! This serializer wraps the functionality of another serializer performing a
//! Base64 encoding and decoding over the obtained byte array.
//!
//! \note This class is thread-safe only when the given serializer type is
//!       thread-safe.
//!
//! \tparam SerializerT Wrapped serializer type (or a data reading type, to use
//!                     the default serializer).
//!
template <typename SerializerT>
class Base64Serializer
    : public AbstractSerializer<
          typename SerializerTraits<SerializerT>::DataReading>
{
public:
    typedef typename SerializerTraits<SerializerT>::Serializer Serializer;

    void serialize(
        const typename SerializerTraits<SerializerT>::DataReading &reading,
        std::vector<unsigned char> &message)
    {
        // Serialize
        std::vector<unsigned char> decoded;
        m_wrapped.serialize(reading, decoded);
        // Encode message
        auto p1 = reinterpret_cast<const char *>(decoded.data());
        std::istringstream decodedStream(std::string(p1, decoded.size()));
        std::stringstream encodedStream;
        base64::encoder encoder;
        encoder.encode(decodedStream, encodedStream);
        // Copy encoded message to vector
        message.resize(((decoded.size() + 2) / 3) * 4);
        auto p2 = reinterpret_cast<char *>(message.data());
        decodedStream.read(p2, message.size());
        message.resize(decodedStream.gcount());
    }

    void
    deserialize(const std::vector<unsigned char> &message,
                typename SerializerTraits<SerializerT>::DataReading &reading)
    {
        // Decode message
        auto p1 = reinterpret_cast<const char *>(message.data());
        std::istringstream encodedStream(std::string(p1, message.size()));
        std::stringstream decodedStream;
        base64::decoder decoder;
        decoder.decode(encodedStream, decodedStream);
        // Copy decoded message to vector
        std::vector<unsigned char> decoded;
        decoded.resize((message.size() * 3) / 4);
        auto p2 = reinterpret_cast<char *>(decoded.data());
        decodedStream.read(p2, decoded.size());
        decoded.resize(decodedStream.gcount());
        // Deserialize
        m_wrapped.deserialize(decoded, reading);
    }

private:
    //! Wrapped serializer.
    Serializer m_wrapped;
};
}

#endif
