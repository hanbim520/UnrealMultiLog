// Copyright ZhangHaiJun 710605420@qq.com, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "HAL/PlatformAtomics.h" // For FPlatformAtomics
#include <atomic>                // Standard C++ atomic library
#include <vector>                // For dynamic array (buffer expansion)
#include <memory>                // For std::unique_ptr

template<typename T, uint32 itialSize = 1024>
class TDynamicConcurrentQueue
{
public:
    TDynamicConcurrentQueue()
        : BufferSize(itialSize), Head(0), Tail(0)
    {
        static_assert((itialSize & (itialSize - 1)) == 0, "InitialSize must be a power of two.");
        Buffer.resize(BufferSize);
    }

    // Adds an item to the queue. Expands if necessary, or overwrites the oldest item.
    void Enqueue(T&& Item)
    {
        uint32 LocalTail = Tail.load(std::memory_order_relaxed);
        uint32 NextTail = (LocalTail + 1) & (BufferSize - 1);

        if (NextTail == Head.load(std::memory_order_acquire))
        {
            // Queue is full, decide whether to overwrite or expand
            if (HasDequeued())
            {
                // Overwrite the oldest item, adjust head
                Head.store((Head.load(std::memory_order_relaxed) + 1) & (BufferSize - 1), std::memory_order_release);
            }
            else
            {
                // Expand the buffer size
                ExpandBuffer();
                NextTail = (Tail.load(std::memory_order_relaxed) + 1) & (BufferSize - 1);
            }
        }

        Buffer[LocalTail] = std::move(Item); // Use std::move here
        Tail.store(NextTail, std::memory_order_release);
    }

    // Removes an item from the queue. Returns false if the queue is empty.
    bool Dequeue(T& OutItem)
    {
        uint32 LocalHead = Head.load(std::memory_order_relaxed);
        if (LocalHead == Tail.load(std::memory_order_acquire))
        {
            return false; // Queue is empty
        }

        OutItem = std::move(Buffer[LocalHead]); // Move the item out
        Head.store((LocalHead + 1) & (BufferSize - 1), std::memory_order_release);
        return true;
    }

    // Check if the queue is empty
    bool IsEmpty() const
    {
        return Head.load(std::memory_order_acquire) == Tail.load(std::memory_order_acquire);
    }

    // Check if the queue is full
    bool IsFull() const
    {
        uint32 LocalTail = Tail.load(std::memory_order_relaxed);
        uint32 NextTail = (LocalTail + 1) & (BufferSize - 1);
        return NextTail == Head.load(std::memory_order_acquire);
    }

private:
    std::vector<T> Buffer;      // Dynamic array for storing items
    uint32 BufferSize;          // The size of the buffer
    std::atomic<uint32> Head;   // Atomic index for the head of the queue
    std::atomic<uint32> Tail;   // Atomic index for the tail of the queue

    // Check if there are any dequeued items
    bool HasDequeued() const
    {
        return Head.load(std::memory_order_acquire) != Tail.load(std::memory_order_acquire);
    }

    // Expands the buffer size by doubling it
    void ExpandBuffer()
    {
        uint32 OldBufferSize = BufferSize;
        BufferSize *= 2;
        std::vector<T> NewBuffer(BufferSize);

        // Copy the existing elements to the new buffer
        uint32 LocalHead = Head.load(std::memory_order_relaxed);
        uint32 LocalTail = Tail.load(std::memory_order_relaxed);

        for (uint32 i = 0; i < OldBufferSize; ++i)
        {
            NewBuffer[i] = std::move(Buffer[(LocalHead + i) & (OldBufferSize - 1)]); // Move instead of copy
        }

        Buffer = MoveTemp(NewBuffer);

        // Reset Head and Tail based on the new buffer
        Head.store(0, std::memory_order_relaxed);
        Tail.store(OldBufferSize, std::memory_order_relaxed);
    }
};
