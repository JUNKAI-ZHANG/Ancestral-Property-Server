//
// Created by haowendeng on 2023/3/2.
//

#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton {
public:
    static T *get() {
        if (Singleton<T>::m_instance == nullptr) Singleton<T>::m_instance = new T();
        return m_instance;
    }

private:
    static T *m_instance;
};

template<typename T>
T* Singleton<T>::m_instance = nullptr;

#endif //SINGLETON_H
