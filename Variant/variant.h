#ifndef VARIANT
#define VARIANT

#include <iostream>
#include <typeindex>


template<size_t arg, size_t ...rest>
struct InterMax;

template<size_t arg>
struct InterMax<arg> : std::integral_constant<size_t, arg>{};

template<size_t arg1, size_t arg2, size_t ...rest>
struct InterMax<arg1, arg2, rest...> : std::integral_constant<size_t, arg1 >= arg2 ? InterMax<arg1, rest...>::value
        : InterMax<arg2, rest...>::value > {};

template<typename ...Args>
struct MaxAlign : std::integral_constant<int, InterMax<std::alignment_of<Args>::value...>::value >{};

template<typename T, typename ...List>
struct Contains;

template<typename T, typename Head, typename...Rest>
struct Contains<T, Head, Rest...>
        : std::conditional<std::is_same<T, Head>::value, std::true_type, Contains<T, Rest...>>::type
{
};

template<typename T>
struct Contains<T> : std::false_type {};

template<typename T, typename ...Rest>
struct IndexOf;

template<typename T, typename Head, typename... Rest>
struct IndexOf<T, Head, Rest...>
{
    enum{ value = IndexOf<T, Rest...>::value + 1 };
};

template<typename T, typename... Rest>
struct IndexOf<T, T, Rest...>
{
    enum{ value = 0 };
};

template<typename T>
struct IndexOf<T>
{
    enum { value = -1 };
};

template<int index, typename... Types>
struct At;

template<int index, typename First, typename... Types>
struct At<index, First, Types...>
{
    using type = typename At<index-1, Types...>::type;
};

template<typename T, typename... Types>
struct At<0, T, Types...>
{
    using type = T;
};


template<typename ...Types>
class Variant
{
    enum
    {
        data_size = InterMax<sizeof(Types)...>::value,
        align_size = MaxAlign<Types...>::value
    };

    using data_t = typename std::aligned_storage<data_size, align_size>::type;

public:

    template<int index>
    using IndexType = typename At<index, Types...>::type;

    Variant(void) : m_type_index(typeid(void)){}
    ~Variant()
    {
        destroy(m_type_index, &m_data);
    }

    Variant(Variant<Types...>&& old) : m_type_index(old.m_type_index)
    {
        move(old.m_type_index, &old.m_data, &m_data);
    }

    Variant(const Variant<Types...>& old) : m_type_index(old.m_type_index)
    {
        copy(old.m_type_index, &old.m_data, &m_data);
    }

    Variant& operator =(const Variant& old)
    {
        if( &old != this)
        {
            copy(old.m_type_index, &old.m_data, &m_data);
            m_type_index = old.m_type_index;
        }

        return *this;
    }

    Variant& operator =(Variant&& old)
    {
        move(old.m_type_index, &old.m_data, &m_data);
        m_type_index = old.m_type_index;
        return *this;
    }

    template<class T,
             class = typename std::enable_if<Contains<typename std::decay<T>::type, Types...>::value>::type>
    Variant(T&& value) : m_type_index(typeid(void))
    {
        destroy(m_type_index, &m_data);
        typedef typename std::decay<T>::type U;
        new(&m_data) U(std::forward<T>(value));
        m_type_index = std::type_index(typeid(U));
    }

    template<typename T>
    bool is()const
    {
        return (m_type_index == std::type_index(typeid(T)));
    }

    bool Empty()const
    {
        return m_type_index == std::type_index(typeid(void));
    }

    std::type_index type()const
    {
        return m_type_index;
    }

    template<typename T>
    typename std::decay<T>::type& get()
    {
        using U = typename std::decay<T>::type;
        if( !is<U>() )
        {
            std::cout << typeid(U).name() << "is not defined. "
                      << "current type is " << m_type_index.name() << std::endl;
            throw std::bad_cast{};
        }

        return *(U*)(&m_data);
    }

    template<typename T>
    int indexOf()
    {
        return IndexOf<T, Types...>::value;
    }

    bool operator ==(const Variant& rhs) const
    {
        return m_type_index == rhs.m_type_index;
    }

    bool operator <(const Variant& rhs) const
    {
        return m_type_index < rhs.m_type_index;
    }


private:
    void destroy(const std::type_index& index, void* buf)
    {
//        [](Types&&...){}( (destroy0<Types>(index, buf), 0)...);
        [](Types&&...){}( destroy0<Types>(index, buf)...);
    }

    template<typename T>
    T destroy0(const std::type_index& id, void* data)
    {
        if( id == std::type_index(typeid(T)) )
            reinterpret_cast<T*>(data)->~T();

        return std::move(T());
    }

    void move(const std::type_index& old_t, void* old_v, void* new_v)
    {
//        [](Types&&...){}( (move0<Types>(old_t, old_v, new_v), 0)... );
        [](Types&&...){}( move0<Types>(old_t, old_v, new_v)... );
    }

    template<typename T>
    T move0(const std::type_index& old_t, void* old_v, void* new_v)
    {
        if( old_t == std::type_index(typeid(T)))
            new(new_v)T( std::move(*reinterpret_cast<T*>(old_v)) );
        return std::move(T());
    }

    void copy(const std::type_index& old_t, const void* old_v, void* new_v)
    {
        [](Types&&...){}( (copy0<Types>(old_t, old_v, new_v), 0)... );
    }

    template<typename T>
    void copy0(const std::type_index& old_t, const void* old_v, void* new_v)
    {
        if( old_t == std::type_index(typeid(T)))
            new(new_v)T(*reinterpret_cast<const T*>(old_v));
    }


    data_t  m_data;
    std::type_index m_type_index;
};

#endif // VARIANT

