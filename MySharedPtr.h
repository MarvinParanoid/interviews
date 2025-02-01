#pragma once

#include <atomic>
#include <iostream>

template<typename T>
class MySharedPtr {
    T *ptr_;
    std::atomic<unsigned> *refCount_;

public:
    MySharedPtr() : ptr_(nullptr), refCount_(nullptr) {
    }

    explicit MySharedPtr(T *ptr) : ptr_(ptr), refCount_(new std::atomic<unsigned>(1)) {
    }

    MySharedPtr(const MySharedPtr &other) : ptr_(other.ptr_), refCount_(other.refCount_) {
        incRefCount();
    }

    MySharedPtr &operator=(const MySharedPtr &other) {
        if (this == &other) { return *this; }
        release();
        ptr_ = other.ptr_;
        refCount_ = other.refCount_;
        incRefCount();
        return *this;
    }

    MySharedPtr(MySharedPtr &&other) noexcept {
        ptr_ = other.ptr_;
        refCount_ = other.refCount_;
        other.ptr_ = nullptr;
        other.refCount_ = nullptr;
    }

    MySharedPtr &operator=(MySharedPtr &&other) noexcept {
        if (this == &other) { return *this; }
        release();
        std::swap(ptr_, other.ptr_);
        std::swap(refCount_, other.refCount_);
        return *this;
    }

    ~MySharedPtr() {
        release();
    }

    void release() {
        if (!refCount_) { return; }
        --(*refCount_);
        if (*refCount_ == 0) {
            delete refCount_;
            delete ptr_;
            ptr_ = nullptr;
            refCount_ = nullptr;
        }
    }

    unsigned getRefCount() const {
        if (refCount_) { return *refCount_; }
        return 0;
    }

    T &operator*() const {
        return *ptr_;
    }

    T *operator->() const { return ptr_; }

    T *get() const {
        return ptr_;
    }

private:
    void incRefCount() {
        if (refCount_) {
            ++(*refCount_);
        }
    }
};
