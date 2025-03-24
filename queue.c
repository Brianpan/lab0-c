#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *ptr = NULL, *safe = NULL;
    list_for_each_entry_safe(ptr, safe, head, list) {
        free(ptr->value);
        free(ptr);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    new_ele->value = strdup(s);
    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    new_ele->value = strdup(s);
    list_add_tail(&new_ele->list, head);
    return true;
}
/* Copy element var */
void copy_to_buf(char **dst, char **src, size_t bufsize)
{
    size_t end = (bufsize - 1);
    if (strlen(*src) >= end) {
        memcpy(*dst, *src, end);
        *dst[end] = '\0';
        return;
    }
    memcpy(*dst, *src, bufsize);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node = head->next;

    element_t *e = list_entry(node, element_t, list);
    if (sp)
        copy_to_buf(&sp, &e->value, bufsize);
    list_del(node);

    return e;
}


/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;

    element_t *e = list_entry(node, element_t, list);
    if (sp)
        copy_to_buf(&sp, &e->value, bufsize);

    list_del(node);

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    struct list_head **ptr = &head;
    int qs = 0;
    while (*ptr && *ptr != head) {
        qs++;
        ptr = &(*ptr)->next;
    }
    return qs;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
