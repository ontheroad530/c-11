#ifndef NONCOPYABLE
#define NONCOPYABLE

class NonCopyable
{
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif // NONCOPYABLE

