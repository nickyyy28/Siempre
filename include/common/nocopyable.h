#ifndef __NOCOPYABLE_H
#define __NOCOPYABLE_H

namespace siem{

class NoCopyAble
{
public:
    NoCopyAble() = default;
    ~NoCopyAble() = default;
private:
    NoCopyAble(const NoCopyAble&) = delete;
    NoCopyAble(const NoCopyAble&&) = delete;
    NoCopyAble& operator=(const NoCopyAble&) = delete;
};

}

#endif //__NOCOPYABLE_H