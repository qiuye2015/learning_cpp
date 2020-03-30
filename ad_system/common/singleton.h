#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<class T>
class SingleTon
{
public:
    static T* Instance()
    {
        if (m_self == nullptr)
        {
            m_self = new T;
        }

        return m_self;
    }


private:
    static T *m_self;

private:
    SingleTon<T>(const SingleTon<T>& anOther);
    SingleTon<T>& operator=(const SingleTon<T>& anOther);
    ~SingleTon<T>();
};


template<class T>
T* SingleTon<T>::m_self = nullptr;

#endif
