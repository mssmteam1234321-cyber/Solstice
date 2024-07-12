#pragma once
#include <cstdint>
#include <string>
#include <atomic>
#define _AMD64_
#include <minwindef.h>
#include <unordered_map>
//
// Created by vastrakai on 6/25/2024.
//


/*template<typename T>
struct SharedCounter {
    /* volatile ? #1# T* value;
    std::atomic<int> shared, weak;

    SharedCounter(T* value) : value(value) {}

    void addSharedRef() { ++shared; }

    void addWeakRef() { --shared; }

    bool releaseSharedRef() {
        if (--shared == 0) {
            if (value != nullptr) {
                T* oldValue = value;
                value = nullptr;
                delete oldValue;
            }
            return (weak == 0);
        }
        return false;
    }

    bool releaseWeakRef() {
        return (--weak == 0 && value);
    }
};

template<typename T>
struct WeakPtr {
    SharedCounter<T>* counter = nullptr;

    WeakPtr(T* val = nullptr) {
        if (val) {
            counter = new SharedCounter<T>(val);
            counter->addWeakRef();
        }
    }

    WeakPtr(WeakPtr&& ptr) {
        counter = std::move(ptr.counter);
        ptr.counter = nullptr;
    }

    WeakPtr& operator=(WeakPtr const& ptr) {
        reset();
        this->counter = ptr.counter;
        if (counter)
            counter->addWeakRef();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& ptr) {
        counter = std::move(ptr.counter);
        ptr.counter = nullptr;
        return *this;
    }

    void reset() {
        if (counter && counter->releaseWeakRef())
            delete counter;
        counter = nullptr;
    }

    ~WeakPtr() {
        reset();
    };

    template <typename... Args>
    static WeakPtr<T> make(Args&&... args) {
        return WeakPtr<T>(new T(std::forward(args...)));
    }

    T& operator*() {
        return *counter->value;
    }

    T* operator->() {
        return counter->value;
    }

    T* get() {
        if (!counter)
            return nullptr;
        return counter->value;
    }

    T const* get() const {
        if (!counter)
            return nullptr;
        return counter->value;
    }
};*/


class MemUtils {
public:
    template<typename Ret, typename... Args>
    static auto getFunc(void* func);
    template<typename Ret, typename... Args>
    static auto getVirtualFunc(void* _this, int index);
    template <typename TRet, typename... TArgs>
    static TRet callFastcall(void* func, TArgs... args)
    {
        using Fn = TRet(__fastcall*)(TArgs...);
        Fn f = reinterpret_cast<Fn>(func);
        return f(args...);
    }
    template <typename TRet, typename... TArgs>
    static TRet callFastcall(uintptr_t func, TArgs... args)
    {
        using Fn = TRet(__fastcall*)(TArgs...);
        Fn f = reinterpret_cast<Fn>(func);
        return f(args...);
    }

    template<typename Ret, typename... Args>
    static auto callVirtualFunc(int index, void* _this, Args... args)
    {
        using Fn = Ret(__thiscall*)(void*, Args...);
        auto vtable = *reinterpret_cast<uintptr_t**>(_this);
        return reinterpret_cast<Fn>(vtable[index])(_this, args...);
    }


#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define CLASS_FIELD(type, name, offset) \
    AS_FIELD(type, name, get##name); \
    type get##name() const { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); } \
    void set##name(type v) const { *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset) = std::move(v); }

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define PAD(size) \
    private: \
        char TOKENPASTE2(padding_, __LINE__) [size]; \
    public:

#define PADDING(size) char TOKENPASTE2(padding_, __LINE__) [size]

    static uintptr_t getAddressByIndex(uintptr_t _this, int index);

    static const std::string getMbMemoryString(uintptr_t addr);

    static uintptr_t GetVTableFunction(void *_this, int index);
    static std::string getModulePath(HMODULE handle);
};


