#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h = malloc(sizeof(struct list_head));
    if (h != NULL)
        INIT_LIST_HEAD(h);

    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *ptr = NULL, *safe = NULL;
    list_for_each_safe(ptr, safe, head) {
        if (ptr == NULL)
            continue;
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
    if (new_ele == NULL)
        return false;
    char *dup_str = strdup(s);
    if (dup_str == NULL) {
        free(new_ele);
        return false;
    }
    new_ele->value = dup_str;
    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    if (new_ele == NULL)
        return false;
    char *dup_str = strdup(s);
    if (dup_str == NULL) {
        free(new_ele);
        return false;
    }
    new_ele->value = dup_str;
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
    // way to reuse q_remove_head
    if (!head || list_empty(head))
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
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
            if (strcmp(ele->value, ele_next->value) == 0) {
                dup = true;
                curr = curr->next;  // can use goto
            } else
                break;
        }
        // put original next as removed node's next
        if (dup) {
            // a new dummy head
            struct list_head *tmp_head = malloc(sizeof(struct list_head));
            INIT_LIST_HEAD(tmp_head);

            *indirect = start->prev->next;

            list_cut_position(tmp_head, start->prev, curr);
            q_free(tmp_head);
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
    head->prev = tail;
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
static void q_merge_two(struct list_head *first,
                        struct list_head *second,
                        bool descend)
{
    if (!first || !second)
        return;
    struct list_head tmp_head;
    INIT_LIST_HEAD(&tmp_head);
    int flag = descend ? -1 : 1;

    while (!list_empty(first) && !list_empty(second)) {
        element_t *first_node = list_first_entry(first, element_t, list),
                  *second_node = list_first_entry(second, element_t, list);

        char *first_val = first_node->value, *second_val = second_node->value;

        element_t *min_node =
            flag * strcmp(first_val, second_val) < 0 ? first_node : second_node;
        list_move_tail(&min_node->list, &tmp_head);
    }

    list_splice_tail_init(first, &tmp_head);
    list_splice_tail_init(second, &tmp_head);

    list_splice(&tmp_head, first);
    return;
}

void q_merge_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    // second list init
    struct list_head tmp_head;
    list_cut_position(&tmp_head, head, slow);

    q_merge_sort(head, descend);
    q_merge_sort(&tmp_head, descend);

    q_merge_two(head, &tmp_head, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    q_merge_sort(head, descend);
}

static int q_remove_nodes(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *prev = head->prev, *curr = prev->prev;
    element_t *prev_node = list_entry(prev, element_t, list);

    int sz = 1;
    int flag = descend ? -1 : 1;
    while (curr != head) {
        struct list_head *next_prev = curr->prev;
        element_t *cur_node = list_entry(curr, element_t, list);
        if (flag * strcmp(cur_node->value, prev_node->value) > 0) {
            list_del(curr);
            free(cur_node->value);
            free(cur_node);
        } else {
            prev = curr;
            prev_node = list_entry(prev, element_t, list);
            sz++;
        }

        curr = next_prev;
    }
    return sz;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_remove_nodes(head, false);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_remove_nodes(head, true);
}

// static void print_queue(struct list_head *head)
// {
//     element_t *entry;
//     list_for_each_entry(entry, head, list) {
//         printf("%s ", entry->value);
//     }
//     printf("\n");
// }

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head)) {
        queue_contex_t *qc = list_entry(head, queue_contex_t, chain);
        return qc->size;
    }

    // q_len > 1
    struct list_head *end = head->prev;

    while (end != head->next) {
        for (struct list_head *start = head->next;
             start->prev != end && start != end;
             start = start->next, end = end->prev) {
            queue_contex_t *start_qc = list_entry(start, queue_contex_t, chain),
                           *end_qc = list_entry(end, queue_contex_t, chain);
            q_merge_two(start_qc->q, end_qc->q, descend);
            start_qc->size += end_qc->size;
            end_qc->size = 0;
        }
    }

    queue_contex_t *first_qc = list_entry(head->next, queue_contex_t, chain);

    return first_qc->size;
}

