#define _GNU_SOURCE
#include "linked_list.h"

pthread_rwlock_t rwLock;

/**
 * Initializes the linked list.
 *
 * @param head A pointer to the head of the list.
 */
void list_init(Node **head, size_t size) {
    mem_init(size);
    *head = NULL;
    pthread_rwlock_init(&rwLock, NULL);
}

/**
 * Inserts a new node at the end of the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data to be inserted.
 */
void list_insert(Node **head, uint16_t data) {
    pthread_rwlock_wrlock(&rwLock);
    Node *newNode = (Node *)mem_alloc(sizeof(Node));
    if (newNode == NULL) {
        printf_red("Memory allocation failed in insert()\n");
        pthread_rwlock_unlock(&rwLock);
        return;
    }

    if (*head == NULL) {
        newNode->data = data;
        newNode->next = *head;
        *head = newNode;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
        newNode->data = data;
        newNode->next = NULL;
    }
    pthread_rwlock_unlock(&rwLock);
}

/**
 * Inserts a new node after a given node.
 *
 * @param prevNode A pointer to the node after which the new node will be
 * inserted.
 * @param data The data to be inserted.
 */
void list_insert_after(Node *prevNode, uint16_t data) {
    if (prevNode == NULL) {
        printf_red("Previous node cannot be NULL\n");
        return;
    }
    Node *newNode = (Node *)mem_alloc(sizeof(Node));
    if (newNode == NULL) {
        printf_red("Memory allocation failed\n");
        return;
    }
    pthread_rwlock_wrlock(&rwLock);
    newNode->data = data;
    newNode->next = prevNode->next;
    prevNode->next = newNode;
    pthread_rwlock_unlock(&rwLock);
}

/**
 * Inserts a new node before a given node.
 *
 * @param head A pointer to the head of the list.
 * @param nextNode A pointer to the node before which the new node will be
 * inserted.
 * @param data The data to be inserted.
 */
void list_insert_before(Node **head, Node *nextNode, uint16_t data) {
    if (nextNode == NULL) {
        printf_red("Previous node cannot be NULL\n");
        return;
    }
    Node *newNode = (Node *)mem_alloc(sizeof(Node));
    if (newNode == NULL) {
        printf_red("%s,%d Memory allocation failed in mem_alloc()\n", __FILE__, __LINE__);
        return;
    }
    newNode->data = data;
    newNode->next = nextNode;
    pthread_rwlock_wrlock(&rwLock);

    if (*head == nextNode) {
        *head = newNode;
        pthread_rwlock_unlock(&rwLock);
        return;
    }

    Node *current = *head;
    while (current != NULL && current->next != nextNode) {
        current = current->next;
    }

    if (current != NULL) {  // crazy if
        current->next = newNode;
    }

    pthread_rwlock_unlock(&rwLock);
}

/**
 * Deletes a node with the given data from the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data of the node to be deleted.
 */
void list_delete(Node **head, uint16_t data) {
    pthread_rwlock_wrlock(&rwLock);
    Node *temp = *head;
    Node *prev = NULL;

    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        mem_free(temp);
        pthread_rwlock_unlock(&rwLock);
        return;
    }

    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        pthread_rwlock_unlock(&rwLock);
        return;
    }
    prev->next = temp->next;
    mem_free(temp);
    pthread_rwlock_unlock(&rwLock);
}

/**
 * Searches for a node with the given data in the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data to search for.
 * @return A pointer to the node with the given data, or NULL if not found.
 */
Node *list_search(Node **head, uint16_t data) {
    pthread_rwlock_rdlock(&rwLock);
    Node *current = *head;
    while (current != NULL) {
        if (current->data == data) {
            pthread_rwlock_unlock(&rwLock);
            return current;
        }
        current = current->next;
    }
    pthread_rwlock_unlock(&rwLock);
    return NULL;
}

/**
 * Displays the entire list.
 *
 * @param head A pointer to the head of the list.
 */
void list_display(Node **head) {
    list_display_range(head, NULL, NULL);
}

/**
 * Displays a selected range of the list.
 *
 * @param head A pointer to the head of the list.
 * @param startNode A pointer to the starting node of the range.
 * @param endNode A pointer to the ending node of the range.
 */
void list_display_range(Node **head, Node *startNode, Node *endNode) {
    pthread_rwlock_rdlock(&rwLock);
    Node *current = startNode;
    if (*head == NULL) {
        printf("[]");
        pthread_rwlock_unlock(&rwLock);
        return;
    }

    if (!current) {
        current = *head;
    }

    if (endNode) {
        endNode = endNode->next;
    }
    printf("[");
    while (current != NULL && current != endNode) {
        printf("%d", current->data);
        current = current->next;
        if (current != endNode) {
            printf(", ");
        }
    }
    printf("]");
    pthread_rwlock_unlock(&rwLock);
}

/**
 * Counts the number of nodes in the list.
 *
 * @param head A pointer to the head of the list.
 * @return The number of nodes in the list.
 */
int list_count_nodes(Node **head) {
    pthread_rwlock_rdlock(&rwLock);
    int count = 0;
    Node *current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    pthread_rwlock_unlock(&rwLock);
    return count;
}

/**
 * Cleans up the list by freeing all nodes.
 *
 * @param head A pointer to the head of the list.
 */
void list_cleanup(Node **head) {
    Node *current = *head;
    Node *next;
    while (current != NULL) {
        next = current->next;
        mem_free(current);
        current = next;
    }
    *head = NULL;
    mem_deinit();
    pthread_rwlock_destroy(&rwLock);
}
