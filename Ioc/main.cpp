#include <iostream>

#include "ioc_container.h"

struct Base
{
    virtual void Func(){}
    virtual ~Base(){}
};
struct DerivedB : public Base
{
    DerivedB(int a, double b):m_a(a),m_b(b)
    {
    }
    void Func()override
    {
        std::cout<<m_a+m_b<< std::endl;
    }
private:
    int m_a;
    double m_b;
};
struct DerivedC : public Base
{
};
struct A
{
    A(Base * ptr) :m_ptr(ptr)
    {
    }

    ~A()
    {
        if(m_ptr!=nullptr)
        {
            delete m_ptr;
            m_ptr = nullptr;
        }
    }

    void Func()
    {
        if( m_ptr )
            m_ptr->Func();
    }

private:
Base * m_ptr;
};


int main()
{
    IocContainer ioc;
    ioc.RegisterType<A, DerivedC>("C");
    ioc.RegisterType<A, DerivedB, int , double>("B");

    auto c = ioc.ResolveShared<A>("C");
    auto b = ioc.ResolveShared<A>("B", 1, 2.2);
    b->Func();

    return 0;
}

