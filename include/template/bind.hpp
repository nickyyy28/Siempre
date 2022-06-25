#ifndef __SIEM_BIND_HPP
#define __SIEM_BIND_HPP

#include <string>
#include <algorithm>
#include <vector>

namespace siem
{

template<class>
class bind;

template<class Func, class Tv>
class bind1st{
public:
    bind1st(Func fun, Tv value)
    {
        m_value = value;
        m_fun = fun;
    }

    bool operator()(Tv v)
    {
        return m_fun(m_value, v);
    }

private:
    Func m_fun;
    Tv m_value;
};

template<class Func, class Tv>
class bind2nd{
public:
    bind2nd(Func fun, Tv value)
    {
        m_value = value;
        m_fun = fun;
    }

    bool operator()(Tv v)
    {
        return m_fun(v, m_value);
    }

private:
    Func m_fun;
    Tv m_value;
};



template<class It, class Func>
It find_if(It begin, It end, Func f)
{
    for (It i = begin ; i != end ; ++i) {
        if (f(*i)) {
            return i;
        }
    }
    return end;
}

// template<class Ret, class Obj, class... Arg>
// class bind(){
// public:
//     bind()
//     {

//     }

// private:

// };

}

#endif // !__SIEM_BIND_HPP