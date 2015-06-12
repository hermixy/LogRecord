#ifndef __LES__LIST__BY__MACHUNLEI__H__
#define __LES__LIST__BY__MACHUNLEI__H__

#ifdef __cplusplus
extern "C"{
#endif

typedef struct les_listnode{
    struct les_listnode* next;
}LES_ListNode, *LES_ListNodePtr;

typedef struct{
    LES_ListNodePtr head;
    LES_ListNodePtr tail;
}LES_List, *LES_ListPtr;

#define LES_ListInit(list)  do{\
        ((LES_ListPtr)(list))->head =\
        ((LES_ListPtr)(list))->tail = NULL; \
    }while(0)

#define LES_ListPut2Head(list, node)    do {\
        if((((LES_ListNodePtr)(node))->next = ((LES_ListPtr)(list))->head) == NULL){\
            ((LES_ListPtr)(list))->head = (LES_ListNodePtr)(node); \
            ((LES_ListPtr)(list))->tail = (LES_ListNodePtr)(node); \
        }\
        else\
            ((LES_ListPtr)(list))->head = (LES_ListNodePtr)(node); \
    } while(0)

#define LES_ListPut2Tail(list, node)    do{\
        ((LES_ListNodePtr)(node))->next = NULL;  \
        if(((LES_ListPtr)(list))->tail == NULL){\
            ((LES_ListPtr)(list))->head = (LES_ListNodePtr)(node);  \
            ((LES_ListPtr)(list))->tail = (LES_ListNodePtr)(node);  \
        }\
        else{\
            ((LES_ListPtr)(list))->tail->next = (LES_ListNodePtr)(node);\
            ((LES_ListPtr)(list))->tail = (LES_ListNodePtr)(node);\
        }\
    }while(0)

#define LES_ListGetHead(list, node) do{\
        if(((node) = (void*)((LES_ListPtr)(list))->head) != NULL){\
            ((LES_ListPtr)(list))->head = ((LES_ListNodePtr)(node))->next;  \
        }\
    }while(0)

#define LES_ListRemove(list, del, prev) do{\
        if(prev == NULL){\
            ((LES_ListPtr)(list))->head = ((LES_ListNodePtr)(del))->next;\
            if(((LES_ListPtr)(list))->tail == (LES_ListNodePtr)(del))\
                ((LES_ListPtr)(list))->tail = NULL;\
        }   \
        else{\
            ((LES_ListNodePtr)(prev))->next = ((LES_ListNodePtr)(del))->next;\
            if(((LES_ListPtr)(list))->tail == (LES_ListNodePtr)(del))\
                ((LES_ListPtr)(list))->tail = (LES_ListNodePtr)(prev);  \
        }\
    }while(0)

#define LES_ListPrevious(list, node, prev)  do{\
        LES_ListNodePtr* temp;  \
        ((LES_ListNodePtr)(prev)) = NULL;   \
        temp = ((LES_ListPtr)(list))->head; \
        if((temp != NULL) && (temp != (node)))\
            while(temp->next != NULL){\
                if(temp->next == (LES_ListNodePtr)(node)){\
                    (LES_ListNodePtr)(prev) = temp; \
                    break;\
                }\
                temp = temp->next;\
            }\
    }while(0)

#define LES_ListFirst(list) (((LES_ListPtr)(list))->head)

#define LES_ListLast(list)  (((LES_ListPtr)(list))->tail)

#define LES_ListNext(list)  (((LES_ListNodePtr)(list))->next)

#define LES_ListIsEmpty(list)   (((LES_ListPtr)(list))->head == NULL)

#ifdef __cplusplus
}
#endif
#endif
