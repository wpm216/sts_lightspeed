//
// Created by gamerpuppy on 7/4/2021.
//

#ifndef STS_LIGHTSPEED_ACTIONQUEUE_H
#define STS_LIGHTSPEED_ACTIONQUEUE_H

#include "sts_common.h"

#include <bitset>
#include <execinfo.h>
#include <functional>
#include <iostream>
#include <cassert>
#include <stdexcept>


namespace sts {

    class BattleContext;
    typedef std::function<void (BattleContext &)> ActionFunction;

    struct Action {
        ActionFunction actFunc;
        bool clearOnCombatVictory = true;

        Action() = default;
        Action(ActionFunction a) : actFunc(std::move(a)) {}
        Action(ActionFunction a, bool b) : actFunc(std::move(a)), clearOnCombatVictory(b) {}
    };

    // Diagnostic helper for empty Action() pushes. Logs a C++ backtrace and
    // the current BattleContext (via g_debug_bc) so the pusher can be found.
    // The push helpers below then throw: every Actions:: factory returns a
    // real actFunc, so an empty push is a bug and must not be skipped
    // silently (dropping it used to leave e.g. poison never ticking).
    // pybind surfaces the throw as RuntimeError. See CLAUDE.md
    // ("Monitor: empty-actFunc diagnostics").
    void logEmptyActionPush(const char *site);


    // Simple deque
    template<int capacity>
    struct ActionQueue {
        friend BattleContext;
        int front = 0;
        int back = 0;
        int size = 0;
        std::bitset<capacity> bits; // for the shouldClear field

#ifdef sts_action_queue_use_raw_array
        ActionFunction arr[capacity];
        ActionQueue() = default;
        ActionQueue(const ActionQueue &rhs) : size(rhs.size), back(rhs.back), front(rhs.front), bits(rhs.bits) {
            int cur = rhs.front;
            for (int i = 0; i < rhs.size; ++i) {
                if (cur >= capacity) {
                    cur = 0;
                }
                arr[cur] = rhs.arr[cur];
            }
        }
#else
        std::array<ActionFunction,capacity> arr;
#endif

        void clear();
        void pushFront(Action a);
        void pushBack(Action a);
        bool isEmpty();
        ActionFunction popFront();
        [[nodiscard]] int getCapacity() const;
    };

    template<int capacity>
    void ActionQueue<capacity>::clear() {
        size = 0;
        back = 0;
        front = 0;
    }

    template <int capacity>
    void ActionQueue<capacity>::pushFront(Action a) {
#ifdef sts_asserts
        assert(size != capacity);
#endif
        if (!a.actFunc) {
            logEmptyActionPush("pushFront");
            throw std::logic_error("ActionQueue::pushFront: empty Action (no actFunc)");
        }
        --front;
        ++size;
        if (front < 0) {
            front = capacity-1;
        }
        bits.set(front, a.clearOnCombatVictory);
        arr[front] = std::move(a.actFunc);
    }

    template<int capacity>
    void ActionQueue<capacity>::pushBack(Action a) {
#ifdef sts_asserts
        if (size >= capacity) {
            assert(false);
        }
#endif
        if (!a.actFunc) {
            logEmptyActionPush("pushBack");
            throw std::logic_error("ActionQueue::pushBack: empty Action (no actFunc)");
        }
        if (back >= capacity) {
            back = 0;
        }
        bits.set(back, a.clearOnCombatVictory);
        arr[back] = std::move(a.actFunc);
        ++back;
        ++size;
    }

    template<int capacity>
    bool ActionQueue<capacity>::isEmpty() {
        return size == 0;
    }

    template<int capacity>
    ActionFunction ActionQueue<capacity>::popFront() {
#ifdef sts_asserts
        assert(size > 0 );
#endif
        ActionFunction a = arr[front];
        ++front;
        --size;
        if (front >= capacity) {
            front = 0;
        }
        return a;
    }

    template<int capacity>
    int ActionQueue<capacity>::getCapacity() const {
        return capacity;
    }

}


#endif //STS_LIGHTSPEED_ACTIONQUEUE_H
