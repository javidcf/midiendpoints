
#ifndef BSF_PROTOBUFPTRDATAREADING_H
#define BSF_PROTOBUFPTRDATAREADING_H

#include <memory>
#include <mutex>

namespace bsf
{

//!
//! \brief Weak singleton.
//!
//! A singleton instance created the first time it is requested and gets and
//! destroyed when it is not used anymore.
//!
template <typename T>
class WeakSingleton
{
public:
    WeakSingleton(const WeakSingleton &) = delete;
    WeakSingleton &operator=(const WeakSingleton &) = delete;

    //! \return Weak singleton instance.
    static std::shared_ptr<T> getInstance()
    {
        std::shared_ptr<T> instancePtr = m_instance.lock();
        if (!instancePtr)
        {
            // Thread-safe enough, since std::weak_ptr::lock works atomically
            std::lock_guard<std::mutex> lock(m_mutex);
            instancePtr = m_instance.lock();
            if (!instancePtr)
            {
                // Allows for use of private ctor and dtor in friend classes
                instancePtr = std::shared_ptr<T>(new T, [](T *t)
                                                 {
                                                     delete t;
                                                 });
                m_instance = instancePtr;
            }
        }
        return instancePtr;
    }

private:
    WeakSingleton()
    {
    }

    //! Singleton instance
    static std::weak_ptr<T> m_instance;
    //! Initialization mutex
    static std::mutex m_mutex;
};

template <typename T>
std::weak_ptr<T> WeakSingleton<T>::m_instance;
template <typename T>
std::mutex WeakSingleton<T>::m_mutex;

//!
//! \brief Singleton.
//!
//! A permanent singleton instance created the first time it is requested.
//!
//! This singleton uses the same instance as WeakSingleton, so both can be used
//! together (and in that case the singleton would never be "weak").
//!
template <typename T>
class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    //! \return Singleton instance.
    static std::shared_ptr<T> getInstance()
    {
        std::call_once(m_flag, []
                       {
                           m_instance = WeakSingleton<T>::getInstance();
                       });
        return m_instance;
    }

private:
    Singleton()
    {
    }

    //! Singleton instance
    static std::shared_ptr<T> m_instance;
    //! Initialization flag
    static std::once_flag m_flag;
};
template <typename T>
std::shared_ptr<T> Singleton<T>::m_instance;
template <typename T>
std::once_flag Singleton<T>::m_flag;

} // bsf

#endif
