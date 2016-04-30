#include <iostream>
#include <functional>

#include "aspect.h"
#include "timer.h"

using namespace std;


struct AA
{
    void Before(int i)
    {
        cout << "Before from AA " << i << std::endl;
    }

    void After(int i)
    {
        cout << "After from AA " << i << std::endl;
    }
};

struct BB
{
    void Before(int i)
    {
        cout << "Before from BB " << i << std::endl;
    }

    void After(int i)
    {
        cout << "After from BB " << i << std::endl;
    }
};

struct CC
{
    void Before(int i)
    {
        cout << "Before from CC " << i << std::endl;
    }

    void After(int i)
    {
        cout << "After from CC " << i << std::endl;
    }
};

struct DD
{
    void Before(int i)
    {
        cout << "Before from DD " << i << std::endl;
    }

    void After(int i)
    {
        cout << "After from DD " << i << std::endl;
    }
};

void gt()
{
    cout << "real gt function" << endl;
}

void ht(int a)
{
    cout << "real hi function: " << a << endl;
}

void testAop()
{
    std::function<void(int)> f = std::bind(&ht, std::placeholders::_1);
    Invoke<AA, BB, CC>(std::function<void(int)>(std::bind(&ht, std::placeholders::_1 )), 1);

    std::cout << "----------------------------------" << std::endl;
}


struct TimeElapsedAspect
{
    void Before(int i)
    {
        m_lastTime = m_t.elapsed();
    }

    void After(int i)
    {
        std::cout << "time elapsed: " << m_t.elapsed() - m_lastTime << std::endl;
    }

private:
    double  m_lastTime;
    Timer   m_t;
};

struct LoggingAspect
{
    void Before(int i)
    {
        std::cout << "entering" << std::endl;
    }

    void After(int i)
    {
        std::cout << "leaving" << std::endl;
    }
};

void foo(int a)
{
    std::cout << "real HT function: " << a << std::endl;
}

int main()
{
//    testAop();

    Invoke<LoggingAspect, TimeElapsedAspect>(&foo, 1);

    std::cout << "------------------------------" << std::endl;

    Invoke<TimeElapsedAspect, LoggingAspect>(&foo, 2);

    return 0;
}

