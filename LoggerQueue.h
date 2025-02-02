#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(std::string_view) = 0;
};

class Logger : ILogger {
public:
    void log(std::string_view msg) override {
        std::cout << msg << std::endl;
    }
};

class LoggerQueue {
    Logger &logger_;
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::jthread thread_;
    std::atomic_bool running_ = true;

    void flush() {
        while (true) {
            std::unique_lock lock(mutex_);
            cv_.wait(lock, [&] { return !running_ || !queue_.empty(); });
            std::queue<std::string> tmp = std::move(queue_);
            lock.unlock();
            while (!tmp.empty()) {
                logger_.log(tmp.front());
                tmp.pop();
            }
            if (!running_) {
                return;
            }
        }
    }

public:
    explicit LoggerQueue(Logger &logger) : logger_(logger), thread_(std::bind(&LoggerQueue::flush, this)) {
    }

    ~LoggerQueue() {
        running_ = false;
        cv_.notify_one();
    }

    void log(std::string log) {
        std::lock_guard lock(mutex_);
        queue_.push(std::move(log));
        cv_.notify_one();
    }
};
