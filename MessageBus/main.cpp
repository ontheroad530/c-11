#include <iostream>

#include "message_bus.h"


MessageBus  g_bus;
const std::string topic = "Drive";
const std::string callBackTopic = "Driveok";

struct Subject
{
    Subject()
    {
        g_bus.Attach([this]{ DriveOk();}, callBackTopic);
    }

    void SendReq(const std::string& topic)
    {
        g_bus.SendReq<void, int>(50, topic);
    }

    void DriveOk()
    {
        std::cout << "drive ok" << std::endl;
    }

};

struct Car
{
    Car()
    {
        g_bus.Attach([this](int speed){ Drive(speed);}, topic);
    }

    void Drive(int speed)
    {
        std::cout << "Car drive " << speed << std::endl;
        g_bus.SendReq<void>(callBackTopic);
    }
};

struct Bus
{
    Bus()
    {
        g_bus.Attach([this](int speed){ Drive(speed);}, topic);
    }

    void Drive(int speed)
    {
        std::cout << "Bus drive " << speed << std::endl;
        g_bus.SendReq<void>(callBackTopic);
    }
};


struct Truck
{
    Truck()
    {
        g_bus.Attach([this](int speed){ Drive(speed);}, topic);
    }

    void Drive(int speed)
    {
        std::cout << "Truck drive " << speed << std::endl;
        g_bus.SendReq<void>(callBackTopic);
    }
};

int main()
{
    Subject subject;
    Car car;
    Bus bus;
    Truck truck;
    subject.SendReq(topic);
    subject.SendReq("");

    return 0;
}

void test(int a)
{
    std::cout << "xxxx: " << a << std::endl;
}

void simple_test()
{
    MessageBus bus;
    bus.Attach([](int a){ std::cout << "no reference " << a << std::endl; });
    bus.Attach( test );
    bus.Attach([](int &a){ std::cout << "lvalue reference " << a << std::endl;});
    bus.Attach([](int &&a){ std::cout << "rvalue reference " << a << std::endl;});
    bus.Attach([](const int &a){ std::cout << "const lvalue reference " << a << std::endl;});
    bus.Attach([](int a){ std::cout << "no reference has return type and key " << a << std::endl; return a; }, "a");

    int i = 2;
    bus.SendReq<void, int>(2);
    bus.SendReq<int, int>(2, "a");
    bus.SendReq<void, int&>(i);
    bus.SendReq<void, const int&>(2);
    bus.SendReq<void, int&&>(2);

    bus.Remove<void, int>();
    bus.Remove<int, int>("a");
    bus.Remove<void, int&>();
    bus.Remove<void, const int&>();
    bus.Remove<void, int&&>();

    bus.SendReq<void, int>(2);
    bus.SendReq<int, int>(2, "a");
    bus.SendReq<void, int&>(i);
    bus.SendReq<void, const int&>(2);
    bus.SendReq<void, int&&>(2);
}

