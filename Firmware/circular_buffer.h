/*
 * SitUp - Circ Buf
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#ifndef CIRCULAR_BUFFER_H_INCLUDED
#define CIRCULAR_BUFFER_H_INCLUDED
#include "util/atomic.h"


/// few notes:
/// -capacity must be 2, 4, 8, 16, 32, 64, 128 or 256
/// -buffer is not protected against overflow, it's up to user to read buffer frequently enough

template<class T, uint8_t capacity>
class circularBuffer {
public:
    circularBuffer() : tail(0), count(0)
    {
    }

    uint8_t isEmpty()
    {
        return count == 0;
    }

    uint8_t isFull()
    {
        return count == capacity;
    }

    uint8_t size()
    {
        return count;
    }

    uint8_t getCapacity()
    {
        return capacity;
    }

    T read()
    {
        T tmp;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            uint8_t t = tail;
            tmp = data[t];
            t++;
            t &= capacity - 1;
            tail = t;
            count--;

        }

        return tmp;
    }

    void write(const T c)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            uint8_t head;
            head = tail + count;
            head &= capacity - 1;
            data[head] = c;
            count++;
        }
    }

    T peak()
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            return data[tail];
        }
    }

private:
    volatile uint8_t tail;
    volatile uint8_t count;
    T data[capacity];
};





#endif // CIRCULAR_BUFFER_H_INCLUDED
