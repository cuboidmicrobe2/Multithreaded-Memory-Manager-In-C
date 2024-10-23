#include "memory_manager.h"

void* memoryPool = NULL;
size_t memorySize = 0;
unsigned char* start = NULL;
unsigned char* end = NULL;

/// @brief sets a bit to 1
/// @param array
/// @param index
void set_bit(unsigned char* array, size_t index) {
    array[index / 8] |= (1 << (index % 8));
}

/// @brief sets a bit to 0
/// @param array
/// @param index
void clear_bit(unsigned char* array, size_t index) {
    array[index / 8] &= ~(1 << (index % 8));
}

/// @brief returns bit
/// @param array
/// @param index
/// @return
bool get_bit(const unsigned char* array, size_t index) {
    return array[index / 8] & (1 << (index % 8));
}

/**
 * Initializes the memory manager with a given size.
 *
 * @param size The size of the memory pool to allocate.
 */
void mem_init(size_t size) {
    memoryPool = malloc(size);
    memorySize = size;
    start = calloc((size + 7) / 8, sizeof(char));
    end = calloc((size + 7) / 8, sizeof(char));
}

/**
 * Allocates a block of memory of the given size from the memory pool.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if the allocation
 * fails.
 */
void* mem_alloc(size_t size) {
    if (size > memorySize) return NULL;
    if (size == 0) return memoryPool; // :(
    size_t nrOfEmptySegments = 0;
    bool isEmpty = true;

    for (size_t i = 0; i < memorySize; i++) {
        if (get_bit(start, i)) {
            isEmpty = false;
        }

        nrOfEmptySegments = (isEmpty) ? nrOfEmptySegments + 1 : 0;
        if (nrOfEmptySegments >= size) {
            set_bit(start, i - size + 1);
            set_bit(end, i);
            return memoryPool + i - size + 1;
        }

        if (get_bit(end, i) == true) {
            isEmpty = true;
        }
    }

    return NULL;
}

/**
 * Frees a previously allocated block of memory.
 *
 * @param block A pointer to the memory block to free.
 */
void mem_free(void* block) {
    if (!block) return;

    size_t index = block - memoryPool;
    if (index >= memorySize || get_bit(start, index) != 1) {
        return;
    }

    clear_bit(start, index);
    while (get_bit(end, index) == 0) index++;
    clear_bit(end, index);
}

/**
 * Resizes a previously allocated block of memory.
 *
 * @param block A pointer to the memory block to resize.
 * @param size The new size of the memory block.
 * @return A pointer to the resized memory block, or NULL if the allocation
 * fails.
 */
void* mem_resize(void* block, size_t size) {
    if (size == 0) {
        mem_free(block);
        return NULL;
    }
    if (!block) return mem_alloc(size);

    size_t startIndex = block - memoryPool;
    if (startIndex >= memorySize || !get_bit(start, startIndex)) return NULL;

    size_t endIndex = startIndex;
    while (!get_bit(end, endIndex)) endIndex++;
    mem_free(block);
    void* resizedBlock = mem_alloc(size);

    if (!resizedBlock) {
        set_bit(start, startIndex);
        set_bit(end, endIndex);
        return NULL;
    }

    if (resizedBlock == block)
        return block;

    else {
        size_t sizeToReplace = (endIndex - startIndex + 1);
        size_t minSize = (size < sizeToReplace) ? size : sizeToReplace;

        memcpy(resizedBlock, block, minSize);
        return resizedBlock;
    }
}

/**
 * Deinitializes the memory manager by freeing all allocated memory blocks and
 * resetting the memory manager state.
 */
void mem_deinit() {
    free(start);
    free(end);
    free(memoryPool);
    memorySize = 0;
}