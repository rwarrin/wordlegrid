#ifndef WORDLE_DICTIONARY_H

struct tst_node
{
    struct tst_node *Left;
    struct tst_node *Right;
    union
    {
        struct tst_node *Equal;
        struct tst_node *Next;
    };
    u32 Character;
    u32 IsWord;
    struct word_list_entry *Entry;

    // NOTE(rick): This is here to keep track of memory blocks so that we can
    // free them later. This should not be used for anything else.
    struct tst_node *NextMemoryBlock_;
};

struct dictionary
{
    struct tst_node *Words;
    struct tst_node *FreeList;
    u32 InitialNodeCount;
    u32 WordCount;

    // NOTE(rick): This is here to keep track of memory blocks so that we can
    // free them later. This should not be used for anything else.
    struct tst_node *Memory_;
};

#define WORDLE_DICTIONARY_H
#endif
