
#ifndef BSF_JSONDATAREADING_H
#define BSF_JSONDATAREADING_H

#include "common.h"
#include "external/json.hpp"

#include <algorithm>
#include <iterator>
#include <istream>
#include <stdexcept>
#include <vector>

namespace bsf
{

//!
//! \brief A JSON document.
//!
//! This class is a typedef of nlohmann::json.
//! \see https://github.com/nlohmann/json
//!
typedef nlohmann::json JsonDataReading;

//! Beginning character of a JSON document.
static const unsigned char JSON_BEGIN{'{'};
//! Ending character of a JSON document.
static const unsigned char JSON_END{'}'};

//!
//! \brief Default serializer for JSON data readings.
//!
template <>
class DefaultSerializer<JsonDataReading>
    : public AbstractSerializer<JsonDataReading>
{
public:
    void serialize(const JsonDataReading &reading,
                   std::vector<unsigned char> &message) const
    {
        auto str = reading.dump();
        auto p = reinterpret_cast<const unsigned char *>(str.data());
        message.resize(str.size());
        rserialized.assign(p, p + str.size());
    }

    void deserialize(const std::vector<unsigned char> &message,
                     JsonDataReading &reading) const
    {
        auto begin = std::find(message.begin(), message.end(), JSON_BEGIN);
        auto end = std::find(message.rbegin(), message.rend(), JSON_END).base();
        if (begin == message.end() || end == message.begin())
        {
            throw SerializationError("The message is not a JSON document");
        }
        auto size = std::distance(begin, end);
        if (size < 1)
        {
            throw SerializationError("The message is not a JSON document");
        }
        auto pBegin = reinterpret_cast<const char *>(&(*begin));
        try
        {
            reading << JsonDataReading::parse(std::string(pBegin, size));
        }
        catch (const std::invalid_argument &)
        {
            throw SerializationError("The message is not a JSON document");
        }
    }
};

} // bsf

#endif
