#ifndef _SINGLETON_H
#define _SINGLETON_H

template <typename T>
class Singleton
{
public:
    static T *get()
    {
        if (Singleton<T>::m_instance == nullptr)
        {
            Singleton<T>::m_instance = new T();
        }
        return m_instance;
    }

private:
    static T *m_instance;
};

template <typename T>
T *Singleton<T>::m_instance = nullptr;

#endif // _SINGLETON_H
