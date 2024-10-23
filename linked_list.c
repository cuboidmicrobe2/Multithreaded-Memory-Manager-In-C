#include "linked_list.h"

/**
 * Initializes the linked list.
 *
 * @param head A pointer to the head of the list.
 */
void list_init(Node **head, size_t size) {
    mem_init(size);
    *head = NULL;
}

/**
 * Inserts a new node at the end of the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data to be inserted.
 */
void list_insert(Node **head, uint16_t data) {
    Node *newNode = (Node *)mem_alloc(sizeof(Node));
    if (newNode == NULL) {
        printf_red("Memory allocation failed in insert()\n");
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
    newNode->data = data;
    newNode->next = prevNode->next;
    prevNode->next = newNode;
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

    if (*head == nextNode) {
        *head = newNode;
        return;
    }

    Node *current = *head;
    while (current != NULL && current->next != nextNode) {
        current = current->next;
    }

    if (current != NULL) {  // crazy if
        current->next = newNode;
    }
}

/**
 * Deletes a node with the given data from the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data of the node to be deleted.
 */
void list_delete(Node **head, uint16_t data) {
    Node *temp = *head;
    Node *prev = NULL;

    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        mem_free(temp);
        return;
    }

    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    prev->next = temp->next;
    mem_free(temp);
}

/**
 * Searches for a node with the given data in the list.
 *
 * @param head A pointer to the head of the list.
 * @param data The data to search for.
 * @return A pointer to the node with the given data, or NULL if not found.
 */
Node *list_search(Node **head, uint16_t data) {
    Node *current = *head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Displays the entire list.
 *
 * @param head A pointer to the head of the list.
 */
void list_display(Node **head) {
    Node *current = *head;
    if (current == NULL) {
        printf("NULL");
        return;
    }

    printf("[");
    while (current != NULL) {
        printf("%d", current->data);
        current = current->next;
        if (current) {
            printf(", ");
        }
    }
    printf("]");
}

/**
 * Displays a selected range of the list.
 *
 * @param head A pointer to the head of the list.
 * @param startNode A pointer to the starting node of the range.
 * @param endNode A pointer to the ending node of the range.
 */
void list_display_range(Node **head, Node *startNode, Node *endNode) {
    Node *current = startNode;
    if (*head == NULL) {
        printf("[]");
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
}

/**
 * Counts the number of nodes in the list.
 *
 * @param head A pointer to the head of the list.
 * @return The number of nodes in the list.
 */
int list_count_nodes(Node **head) {
    int count = 0;
    Node *current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
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
}
