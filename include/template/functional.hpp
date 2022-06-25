#ifndef __SIEM_FUNCTIONAL_HPP
#define __SIEM_FUNCTIONAL_HPP

namespace siem{

template<class>
class function;

template <class Ret, class... Arg>
class function<Ret(Arg...)>{
public:
    using pFunc = Ret(*)(Arg...);

    function(pFunc pfunc)
    {
        _pfunc = pfunc;
    }

    Ret operator ()(Arg... arg)
    {
        return _pfunc(arg...);
    }

private:
    pFunc _pfunc;
};

// template<class Ret, class Obj, class... Arg>
// class function<Ret(Obj, Arg...)>{
// public:
//     using pObj = Obj*;
//     using pFunc = Ret(*)(Arg...);

//     function(pFunc pfunc)
//     {
//         _pfunc = pfunc;
//     }

//     Ret operator ()(pObj obj, Arg... arg)
//     {
//         return (pFunc)(obj->*)(arg ...);
//     }
// private:
//     pFunc _pfunc;

// };

}

#endif // !__SIEM_FUNCTIONAL_HPP