
#ifndef BSF_COMMON_H
#define BSF_COMMON_H

#include "Singleton.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace bsf
{

//! A token for a registered callback
typedef unsigned int HandlerToken;

template <typename DataReadingT>
struct Serializer;

template <typename DataReadingT>
struct Deserializer;

//!
//! \brief Default reading factory
//!
template <typename DataReadingT>
class AbstractDataReadingFactory
{
public:
    //! Data reading type.
    typedef DataReadingT DataReading;
};

//!
//! \brief Default reading factory
//!
template <typename DataReadingT>
class DefaultDataReadingFactory
    : public AbstractDataReadingFactory<DataReadingT>
{
public:
    //!
    //! \return A new data reading object
    //!
    DataReadingT newDataReading()
    {
        return DataReadingT();
    }
};

//!
//! \brief Error during data reading serialization or deserialization.
//!
class SerializationError : public std::runtime_error
{
public:
    SerializationError()
    : std::runtime_error("Invalid serialization")
    {
    }

    SerializationError(const std::string &what)
    : std::runtime_error(what)
    {
    }
    SerializationError(const char *what)
    : std::runtime_error(what)
    {
    }
};
}

#endif
