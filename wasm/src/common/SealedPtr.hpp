// sealed_ptr.hpp - A debug-safe wrapper for manually-managed raw pointers
#pragma once
#ifndef SEALED_PTR_HPP
#define SEALED_PTR_HPP

// 启用条件：DEBUG 或强制启用
#if defined(DEBUG) || defined(SEALED_PTR_ALWAYS_ENABLE)
    #define SEALED_PTR_ENABLED
#endif

#include <cstdint>
#include <stdexcept>
#include <iostream>

// ------------------ 异常定义 ------------------

class SegmentationError : public std::runtime_error {
public:
    explicit SegmentationError(const char* msg)
        : std::runtime_error(msg) {
        std::cerr << "[SEALED_PTR] " << msg
                  << " (Access intercepted in debug mode.)" << std::endl;
    }
};

class NullPointerException : public SegmentationError {
public:
    explicit NullPointerException(const char* msg)
        : SegmentationError(msg) {}
};

class WildPointerException : public SegmentationError {
public:
    explicit WildPointerException(const char* msg)
        : SegmentationError(msg) {}
};

class DanglingPointerException : public SegmentationError {
public:
    explicit DanglingPointerException(const char* msg)
        : SegmentationError(msg) {}
};

// ------------------ 状态枚举 ------------------

template<typename T>
class SealedPtr {
protected:
    #ifdef SEALED_PTR_ENABLED
        SealedPtr() noexcept : raw(nullptr), m_state(State::UNINITIALIZED) {}

        explicit SealedPtr(T* p) noexcept : raw(p) {
            m_state = (p ? State::VALID : State::NULL_POINTER);
        }
    #else
        SealedPtr() noexcept : raw(nullptr) {}

        explicit SealedPtr(T* p) noexcept : raw(p) {}
    #endif
public:
    enum class State : uint8_t {
        UNINITIALIZED = 0,  // 未初始化
        NULL_POINTER  = 1,  // 显式为 nullptr
        VALID         = 2,  // 指向有效内存
        DANGLING      = 3,  // 已释放，不可再访问
        DISABLED      = 4   // 未启用
    };

    // -------- 析构 / 赋值 --------
    template<typename... Args>
    static SealedPtr<T> create(Args&&... args) {
        T* ptr = new T(std::forward<Args>(args)...);
        return SealedPtr<T>(ptr);
    }

    ~SealedPtr() noexcept {
        #ifdef SEALED_PTR_ENABLED
            if (m_state == State::VALID) {
                delete raw;
            }
        #else
            delete raw;
        #endif
    }

    SealedPtr(const SealedPtr&) = delete;
    SealedPtr& operator=(const SealedPtr&) = delete;

    // -------- 状态查询 --------
    
    #ifdef SEALED_PTR_ENABLED
        State state() const noexcept { return m_state; }

        bool is_null()     const noexcept { return m_state == State::NULL_POINTER; }
        bool is_valid()    const noexcept { return m_state == State::VALID; }
        bool is_dangling() const noexcept { return m_state == State::DANGLING; }
        bool is_initialized() const noexcept { return m_state != State::UNINITIALIZED; }
    #else
        State state() const noexcept { return State::DISABLED; }

        bool is_null()     const noexcept { return !(bool)raw; }
        bool is_valid()    const noexcept { return (bool)raw; }
        bool is_dangling() const noexcept { return false; }
        bool is_initialized() const noexcept { return true; }
    #endif
    explicit operator bool() const { return is_valid(); }
    // -------- 指针访问（带保护）--------

    T* get() const {
        #ifdef SEALED_PTR_ENABLED
            switch (m_state) {
                case State::NULL_POINTER:     return nullptr;
                case State::UNINITIALIZED:    throw WildPointerException("Uninitialized SealedPtr accessed.");
                case State::DANGLING:         throw DanglingPointerException("Dangling pointer accessed.");
                default:                      break; // VALID
            }
        #endif
        return raw;
    }

    T& operator*() const {
        #ifdef SEALED_PTR_ENABLED
            if (m_state == State::NULL_POINTER) {
                throw NullPointerException("Null pointer dereferenced via *ptr.");
            }
        #endif
        return *get();
    }

    T* operator->() const {
        #ifdef SEALED_PTR_ENABLED
            if (m_state == State::NULL_POINTER) {
                throw NullPointerException("Null pointer accessed via ->.");
            }
        #endif
        return get();
    }

    // -------- 手动生命周期控制 --------

    void destroy() {
        #ifdef SEALED_PTR_ENABLED
            if (m_state != State::VALID) {
                throw DanglingPointerException("Attempted to destroy non-valid pointer (already null/dangling).");
            }
        #endif
        if (raw) {
            delete raw;
            raw = nullptr;
        }
        #ifdef SEALED_PTR_ENABLED
            m_state = State::DANGLING;
        #endif
    }

    // 释放所有权，返回原始指针（移交控制权）
    T* release() noexcept {
        #ifdef SEALED_PTR_ENABLED
            T* p = (m_state == State::VALID || m_state == State::NULL_POINTER) ? raw : nullptr;
            m_state = State::DANGLING;
            raw = nullptr;
            return p;
        #else
            T* p = raw;
            raw = nullptr;
            return p;
        #endif
    }

private:
    T* raw;
    #ifdef SEALED_PTR_ENABLED
        State m_state;
    #endif
};

#endif // SEALED_PTR_HPP