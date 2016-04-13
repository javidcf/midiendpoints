
#ifndef BSF_COMMON_H
#define BSF_COMMON_H

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace bsf
{

//! A token for a registered callback
typedef unsigned int HandlerToken;

//!
//! \brief Abstract serializer.
//!
//! Serializers must implement the operations:
//!   - `void serialize(const DataReadingT &reading,
//!      std::vector<unsigned char> &message)`: Serializes a data reading as a
//!      vector of bytes.
//!   - `void deserialize(const std::vector<unsigned char> &message,
//!      DataReadingT &reading)`: Deserializes a data reading from a vector of
//!      bytes.
//!
//! \tparam The generated data reading type
//!
template <typename DataReadingT>
class AbstractSerializer
{
public:
    //! Data reading type.
    typedef DataReadingT DataReading;
};

//!
//! \brief Default serializer.
//!
template <typename DataReadingT>
class DefaultSerializer : public AbstractSerializer<DataReadingT>
{
};

//!
//! \brief Static type checking type for serializers.
//!
//! \tparam SerializerT A type to be checked whether it is a serializer
//!
template <typename SerializerT>
struct IsSerializer
{
private:
    typedef char(&yes)[1];
    typedef char(&no)[2];
    template <typename S>
    static yes test(typename S::DataReading *);
    template <typename S>
    static no test( ... );

public:
    //! True if the given type is a serializer; false otherwise.
    static const bool value = sizeof(test<SerializerT>(nullptr)) == sizeof(yes);
};

//!
//! \brief Traits of the default serializer of a data reading type.
//!
//! \tparam DataReadingT A data reading type
//!
template <typename DataReadingT, typename U = void>
struct SerializerTraits
{
    //! The default serializer type for the data reading.
    typedef DefaultSerializer<DataReadingT> Serializer;
    //! The data reading type.
    typedef DataReadingT DataReading;
};

//!
//! \brief Traits of an arbitrary serializer type.
//!
//! \tparam SerializerT A serializer type
//!
template <typename SerializerT>
struct SerializerTraits<
    SerializerT,
    typename std::enable_if<IsSerializer<SerializerT>::value>::type>
{
    //! The serializer type.
    typedef SerializerT Serializer;
    //! The data reading type of the serializer.
    typedef typename SerializerT::DataReading DataReading;
};

//!
//! \brief Abstract data reading factory.
//!
//! Data reading factories must implement the operations:
//!   - `DataReadingT newDataReading()`: returns a new instance of the data
//!      reading type.
//!
//! \tparam The generated data reading type
//!
template <typename DataReadingT>
class AbstractDataReadingFactory
{
public:
    //! Data reading type.
    typedef DataReadingT DataReading;
};

//!
//! \brief Default reading factory.
//!
//! \tparam The generated data reading type
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
