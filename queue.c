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
    struct list_head *ptr = NULL, *safe = NULL;
    list_for_each_safe(ptr, safe, head) {
        element_t *ele = list_entry(ptr, element_t, list);
        list_del(ptr);
        free(ele->value);
        free(ele);
    }
    // head must be freed too
    free(head);
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
    if (!head || list_empty(head))
        return 0;
    struct list_head **ptr = &head->next;
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
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head;
    struct list_head *fast = head;

    do {
        slow = slow->next;
        fast = fast->next->next;
    } while (fast != head && fast->next != head);

    list_del(slow);

    // remove entry
    element_t *e = list_entry(slow, element_t, list);
    free(e->value);
    free(e);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head **indirect = &head->next;

    while (*indirect != head) {
        bool dup = false;
        struct list_head *start = *indirect;
        struct list_head *curr = *indirect;
        // compare element to find end
        while (curr->next != head) {
            element_t *ele = list_entry(curr, element_t, list);
            element_t *ele_next = list_entry(curr->next, element_t, list);
            if (!strcmp(ele->value, ele_next->value)) {
                dup = true;
                curr = curr->next;  // can use goto
                continue;
            } else if (dup)
                break;
            curr = curr->next;
        }
        // put original next as removed node's next
        if (dup) {
            // a new dummy head
            struct list_head *tmp_head = malloc(sizeof(struct list_head));
            INIT_LIST_HEAD(tmp_head);
            tmp_head->next = start;
            tmp_head->prev = curr;

            *indirect = curr->next;
            curr->next->prev = start->prev;

            start->prev = tmp_head;
            curr->next = tmp_head;
            q_free(tmp_head);
            dup = false;
        } else
            indirect = &(*indirect)->next;
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head **indirect = &head->next;
    while (*indirect != head && (*indirect)->next != head) {
        struct list_head *first = *indirect;
        struct list_head *second = (*indirect)->next;

        first->next = second->next;
        second->next->prev = first;

        first->prev->next = second;
        second->prev = first->prev;

        first->prev = second;
        second->next = first;

        // prev's next to second
        *indirect = second;
        // move to original b's next ptr
        indirect = &first->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || head->next->next == head)
        return;

    struct list_head *cur = head->next;
    struct list_head *tail = cur;
    while (cur != head) {
        struct list_head *next = cur->next;

        head->next->prev = cur;
        cur->next = head->next;
        cur->prev = head;
        head->next = cur;

        cur = next;
    }
    // last one is the original's next
    tail->next = head;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (k <= 1)
        return;
    int sz = q_size(head);

    struct list_head *group_head = head;
    struct list_head *first = NULL;
    while (sz / k > 0) {
        // reverse
        first = group_head->next;
        struct list_head *cur = first->next;
        // inner reverse
        int inner_k = k - 1;
        while (inner_k > 0) {
            struct list_head *next = cur->next;
            group_head->next->prev = cur;
            cur->next = group_head->next;
            cur->prev = group_head;
            group_head->next = cur;

            cur = next;
            inner_k--;
        }
        // next round
        sz -= k;
        first->next = cur;
        if (sz == 0)
            break;
        group_head = first;
        cur->next->prev = group_head;
    }

    // head prev change
    if (sz == 0)
        head->prev = first;
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

