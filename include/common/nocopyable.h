#ifndef __NOCOPYABLE_H
#define __NOCOPYABLE_H

namespace siem{

class NoCopyAble
{
public:
    NoCopyAble() = default;
    NoCopyAble(const NoCopyAble&) = delete;
    
    NoCopyAble& operator=(const NoCopyAble&) = delete;

    ~NoCopyAble() = default;
};

}

#endif //__NOCOPYABLE_H