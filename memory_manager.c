#include "memory_manager.h"

typedef struct Node {
    void* start;
    void* end;
    struct Node* next;
} Node;

void* memoryPool = NULL;
size_t memorySize = 0;
Node* head = NULL;
pthread_mutex_t mLock;

/**
 * Initializes the memory manager with a given size.
 *
 * @param size The size of the memory pool to allocate.
 */
void mem_init(size_t size) {
    memoryPool = malloc(size);
    memorySize = size;
    head = NULL;
    pthread_mutex_init(&mLock, NULL);
}

/**
 * Allocates a block of memory of the given size from the memory pool.
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if the allocation
 * fails.
 */
void* mem_alloc_no_lock(size_t size) {
    if (size > memorySize) return NULL;
    if (size == 0) return memoryPool;  // :(

    Node* nodeToAdd = malloc(sizeof(Node));
    if (!nodeToAdd) return NULL;

    if (head == NULL || head->start - memoryPool >= size) {
        nodeToAdd->start = memoryPool;
        nodeToAdd->end = memoryPool + size;
        nodeToAdd->next = head;
        head = nodeToAdd;
        return nodeToAdd->start;
    }

    Node* walker = head;
    while (walker->next != NULL) {
        if (walker->next->start - walker->end >= size) {
            nodeToAdd->start = walker->end;
            nodeToAdd->end = walker->end + size;
            nodeToAdd->next = walker->next;
            walker->next = nodeToAdd;
            return nodeToAdd->start;
        }
        walker = walker->next;
    }

    if (memoryPool + memorySize - walker->end >= size) {
        nodeToAdd->start = walker->end;
        nodeToAdd->end = walker->end + size;
        nodeToAdd->next = NULL;
        walker->next = nodeToAdd;
        return nodeToAdd->start;
    }

    free(nodeToAdd);
    return NULL;
}

void* mem_alloc(size_t size) {
    if (size > memorySize) return NULL;
    if (size == 0) return memoryPool;  // :(

    Node* nodeToAdd = malloc(sizeof(Node));
    if (!nodeToAdd) return NULL;

    pthread_mutex_lock(&mLock);

    if (head == NULL || head->start - memoryPool >= size) {
        nodeToAdd->start = memoryPool;
        nodeToAdd->end = memoryPool + size;
        nodeToAdd->next = head;
        head = nodeToAdd;

        pthread_mutex_unlock(&mLock);
        return nodeToAdd->start;
    }

    Node* walker = head;
    while (walker->next != NULL) {
        if (walker->next->start - walker->end >= size) {
            nodeToAdd->start = walker->end;
            nodeToAdd->end = walker->end + size;
            nodeToAdd->next = walker->next;
            walker->next = nodeToAdd;

            pthread_mutex_unlock(&mLock);
            return nodeToAdd->start;
        }
        walker = walker->next;
    }

    if (memoryPool + memorySize - walker->end >= size) {
        nodeToAdd->start = walker->end;
        nodeToAdd->end = walker->end + size;
        nodeToAdd->next = NULL;
        walker->next = nodeToAdd;

        pthread_mutex_unlock(&mLock);
        return nodeToAdd->start;
    }

    free(nodeToAdd);

    pthread_mutex_unlock(&mLock);
    return NULL;
}

/**
 * Frees a previously allocated block of memory.
 *
 * @param block A pointer to the memory block to free.
 */
void mem_free(void* block) {
    pthread_mutex_lock(&mLock);
    if (!block || !head) {
        pthread_mutex_unlock(&mLock);
        return;
    }

    Node* curr = head;
    Node* prev = NULL;

    while (curr != NULL) {
        if (curr->start == block) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);

            pthread_mutex_unlock(&mLock);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&mLock);
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
    if (!block) {
        return mem_alloc(size);
    }

    if (size == 0) {
        mem_free(block);
        return NULL;
    }

    pthread_mutex_lock(&mLock);

    Node* walker = head;
    Node* prev = NULL;

    while (walker != NULL && walker->start != block) {
        prev = walker;
        walker = walker->next;
    }

    if (walker == NULL) {
        // Block not found
        pthread_mutex_unlock(&mLock);
        return NULL;
    }

    size_t oldSize = walker->end - walker->start;

    // Remove the old block from the list
    if (prev == NULL) {
        head = walker->next;
    } else {
        prev->next = walker->next;
    }

    // Allocate a new block with the new size
    void* newBlock = mem_alloc_no_lock(size);
    if (newBlock) {
        memcpy(newBlock, block, (size < oldSize) ? size : oldSize);
        free(walker);

        pthread_mutex_unlock(&mLock);
        return newBlock;
    } else {
        // Allocation failed, restore the old block
        if (prev == NULL) {
            head = walker;
        } else {
            prev->next = walker;
        }

        pthread_mutex_unlock(&mLock);
        return NULL;
    }
}

/**
 * Deinitializes the memory manager by freeing all allocated memory blocks and
 * resetting the memory manager state.
 */
void mem_deinit() {
    Node* curr = head;
    while (curr != NULL) {
        Node* next = curr->next;
        free(curr);
        curr = next;
    }
    free(memoryPool);
    memoryPool = NULL;
    memorySize = 0;
    head = NULL;
    pthread_mutex_destroy(&mLock);
}