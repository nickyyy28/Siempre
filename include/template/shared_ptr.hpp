#ifndef __SIEM_SHARED_PTR_HPP
#define __SIEM_SHARED_PTR_HPP

#include <iostream>
#include <stdexcept>
#include <atomic>

namespace siem {

template<class T>
class TypeWrapper;

template<class T>
class shared_ptr;

template <class T>
class weak_ptr;

template<class T>
class unique_ptr;

template<class T>
class TypeWrapper{
    friend class shared_ptr<T>;
    friend class weak_ptr<T>;
public:
    TypeWrapper(T *val = nullptr)
    {
        m_val = val;
        m_use_count = 1;
        m_ref_count = 1;
    }

    ~TypeWrapper()
    {
        m_use_count = 0;
        if (m_val) {
            delete m_val;
            m_val = nullptr;
        }
    }

    void destroy()
    {
        m_use_count = 0;
        if (m_val) {
            delete m_val;
            m_val = nullptr;
        }
    }

    void addUse()
    {
        m_use_count ++;
        addRef();
    }

    void delUse()
    {
        delRef();
        m_use_count --;
    }

    void addRef()
    {
        m_ref_count ++;
    }

    void delRef()
    {
        m_ref_count --;
    }

    int getRef() const
    {
        return m_ref_count;
    }

private:
    T *m_val;
    int m_use_count = 0;
    int m_ref_count = 0;
};

template<class T>
class shared_ptr{
    friend class weak_ptr<T>;
public:
    shared_ptr(T * ptr = nullptr)
    {
        m_wrap = new TypeWrapper<T>(ptr);
    }
    
    shared_ptr(shared_ptr<T>& ptr)
    {
        m_wrap = ptr.m_wrap;
        m_wrap->addUse();
    }

    shared_ptr<T>& operator=(shared_ptr<T>& ptr)
    {
        if (use_count()) {
            m_wrap->delUse();
            if (use_count() == 0) {
                m_wrap->destroy();
            }
        }

        if (m_wrap->getRef() == 0) {
            delete m_wrap;
        }

        m_wrap = ptr.m_wrap;

        m_wrap->addUse();
        return *this;
    }

    T& operator*()
    {
        if (*this) {
            return *(m_wrap->m_val);
        } else {
            throw std::invalid_argument("NullPointer ERROR");
        }
    }

    operator bool() const noexcept
    {
        return ((m_wrap->m_val) && (!!use_count()));
    }

    T* operator->()
    {
        if (*this) {
            return m_wrap->m_val;
        } else {
            throw std::invalid_argument("NullPointer ERROR");
        }
    }

    T& get()
    {
        if (*this) {
            return *m_wrap->m_val;
        } else {
            throw std::invalid_argument("NullPointer ERROR");
        }
    }

    int use_count() const 
    {
        return m_wrap->m_use_count;
    }

    void reset(T* ptr = nullptr)
    {
        if (use_count()) {
            m_wrap->delUse();
            if (use_count() == 0) {
                m_wrap->destroy();
            }
        }

        if (m_wrap->getRef() == 0) {
            delete m_wrap;
        }
        
        m_wrap = new TypeWrapper<T>(ptr);
    }

    void reset(shared_ptr<T>& ptr)
    {
        if (use_count()) {
            m_wrap->delUse();
            if (m_wrap->m_use_count == 0) {
                m_wrap->destroy();
            }
        }

        if (m_wrap->getRef() == 0) {
            delete m_wrap;
        }

        m_wrap = ptr.m_wrap;

        m_wrap->addUse();
    }

    void swap(shared_ptr<T>& ptr)
    {
        auto temp = ptr.m_wrap;
        ptr.m_wrap = m_wrap;
        m_wrap = temp;
    }
    
    ~shared_ptr()
    {
        if (use_count()) {
            m_wrap->delUse();
            if (use_count() == 0) {
                // delete m_wrap;
                m_wrap->destroy();
            }
        }

        if (m_wrap->getRef() == 0) {
            delete m_wrap;
        }
    }
private:
    TypeWrapper<T> *m_wrap;
};

template <class T>
class weak_ptr {
public:
    weak_ptr(const shared_ptr<T>& ptr)
    {
        m_wrap = ptr.m_wrap;
        m_wrap->addRef();
    }

    weak_ptr(const weak_ptr<T>& ptr)
    {
        m_wrap = ptr.m_wrap;
        m_wrap->addRef();
    }

    weak_ptr()
    {
        m_wrap = nullptr;
    }

    ~weak_ptr()
    {
        m_wrap->delRef();
        if (m_wrap->getRef() == 0) {
            delete m_wrap;
        }
    }

    weak_ptr<T>& operator=(const shared_ptr<T>& ptr)
    {
        m_wrap->delRef();
        if (m_wrap->getRef() == 0) {
            delete m_wrap;
            m_wrap = nullptr;
        }

        m_wrap = ptr.m_wrap;
        m_wrap->addRef();

        return *this;
    }

    weak_ptr<T>& operator=(const weak_ptr<T>& ptr)
    {
        m_wrap->delRef();
        if (m_wrap->getRef() == 0) {
            delete m_wrap;
            m_wrap = nullptr;
        }

        m_wrap = ptr.m_wrap;
        m_wrap->addRef();

        return *this;
    }

    int use_count() const
    {
        if (m_wrap == nullptr) {
            return 0;
        }
        return m_wrap->m_use_count;
    }

    explicit operator bool() const noexcept
    {
        return (m_wrap && (m_wrap->m_val) && (!!use_count()));
    }

    void swap(weak_ptr<T>& ptr)
    {
        auto temp = ptr.m_wrap;
        ptr.m_wrap = m_wrap;
        m_wrap = temp;
    }

private:
    TypeWrapper<T> *m_wrap;
};



}

#endif //__SIEM_SHARED_PTR_HPP