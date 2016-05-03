#ifndef IOC_CONTAINER
#define IOC_CONTAINER

#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <functional>

#include "MessageBus/any.h"
#include "MessageBus/noncopyable.h"


class IocContainer : NonCopyable
{
public:

    template<class T, typename Depend, typename... Args>
    typename std::enable_if<!std::is_base_of<T, Depend>::value>::type
    RegisterType(const std::string& strKey)
    {
        std::function<T*(Args...)> func = [](Args... args){ return new T( new Depend(args...) ); };
        RegisterType(strKey, func);
    }

    template<class T, typename Depend, typename... Args>
    typename std::enable_if<std::is_base_of<T, Depend>::value>::type
    RegisterType(const std::string &strKey)
    {
        std::function<T*(Args...)> func = [](Args... args){ return new Depend(args...); };
        RegisterType(strKey, func);
    }

    template<class T, typename... Args>
    void RegisterSimple(const std::string& strKey)
    {
        std::function<T*(Args...)> func = [](Args... args){ return new T(args...); };
        RegisterType(strKey, func);
    }

    template<class T, typename... Args>
    T* Resolve(const std::string& strKey, Args... args)
    {
        auto it = m_creatorMap.find(strKey);
        if( it == m_creatorMap.end() )
            return nullptr;

        Any resolver = it->second;
        std::function<T*(Args...)> func = resolver.AnyCast<std::function<T*(Args...)>>();
        return func(args...);
    }

    template<class T, typename... Args>
    std::shared_ptr<T> ResolveShared(const std::string& strKey, Args... args)
    {
        T* t = Resolve<T>(strKey, args...);
        return std::shared_ptr<T>(t);
    }

private:
    void RegisterType(const std::string& strKey, Any constructor)
    {
        if(m_creatorMap.find(strKey) != m_creatorMap.end() )
            throw std::invalid_argument("this key has already exist");
        m_creatorMap.emplace(strKey, constructor);
    }

private:
    std::unordered_map<std::string, Any>        m_creatorMap;
};

#endif // IOC_CONTAINER

