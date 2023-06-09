#ifndef _THREAD_SAFETY_QUEUE
#define _THREAD_SAFETY_QUEUE

#include <mutex>
#include <queue>
#include <memory>
#include <string>
#include <iostream>
#include <condition_variable>

template <class T, class Container = std::queue<T>>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() = default;

    template <class Element>
    void Push(Element &&element)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::forward<Element>(element));
        not_empty_cv_.notify_one();
    }

    void WaitAndPop(T &t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_cv_.wait(lock, [this]()
                           { return !queue_.empty(); });

        t = std::move(queue_.front());
        queue_.pop();
    }

    std::shared_ptr<T> WaitAndPop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_cv_.wait(lock, [this]()
                           { return !queue_.empty(); });

        std::shared_ptr<T> t_ptr = std::make_shared<T>(queue_.front());
        queue_.pop();

        return t_ptr;
    }

    bool TryPop(T &t)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return false;
        }

        t = std::move(queue_.front());
        queue_.pop();

        return true;
    }

    std::shared_ptr<T> TryPop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> t_ptr = std::make_shared<T>(queue_.front());
        queue_.pop();

        return t_ptr;
    }

    bool IsEmpty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    ThreadSafeQueue(const ThreadSafeQueue &) = delete;
    ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;
    ThreadSafeQueue(ThreadSafeQueue &&) = delete;
    ThreadSafeQueue &operator=(ThreadSafeQueue &&) = delete;

private:
    Container queue_;

    std::condition_variable not_empty_cv_;
    mutable std::mutex mutex_;
};

#endif