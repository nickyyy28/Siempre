#ifndef __SIEM_SHARED_PTR_HPP
#define __SIEM_SHARED_PTR_HPP

namespace {

template<class T>
class TypeWrapper{
public:
    TypeWrapper(T *val)
    {
        if (val) {
            m_val = *val;
            m_use_count = 1;
        } else {
            throw std::logic_error("nullptr excpetion");
        }
    }

    ~TypeWrapper()
    {
        m_use_count =0;
    }

    const TypeWrapper<T>& operator--(int)
    {
        m_use_count --;
        return *this;
    }

    TypeWrapper<T> operator--()
    {
        TypeWrapper<T> w = *this;
        m_use_count --;
        return w;
    }

private:
    T m_val;
    int m_use_count = 0;
};

template<class T>
class shared_ptr{
public:
    shared_ptr(T * val)
    {
        m_wrap = new TypeWrapper<T>(val);
    }
    
    ~shared_ptr()
    {
        if (!--*m_wrap) {
            delete m_wrap;
        }
    }
private:
    TypeWrapper<T> *m_wrap = nullptr;
};

}

#endif //__SIEM_SHARED_PTR_HPP