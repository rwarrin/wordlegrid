#include "wordle_dictionary.h"

// NOTE(rick): Implementation for a Ternary Search Tree (TST) for fast insertion
// and search of strings.

static struct tst_node *
GetNewFreeList(u32 Count)
{
    struct tst_node *Result = (struct tst_node *)malloc(sizeof(*Result)*Count);

    for(u32 Index = 0; Index < (Count - 1); ++Index)
    {
        struct tst_node *ThisNode = Result + Index;
        ThisNode->Next = ThisNode + 1;
    }

    {
        struct tst_node *Node = Result + (Count - 1);
        Node->Next = 0;
    }

    return(Result);
}

static struct tst_node *
DictionaryGetNewTSTNode(struct dictionary *Dictionary)
{
    struct tst_node *Result = 0;
    if(!Dictionary->FreeList)
    {
        Dictionary->FreeList = GetNewFreeList(Dictionary->InitialNodeCount);
        Dictionary->FreeList->NextMemoryBlock_ = Dictionary->Memory_;
        Dictionary->Memory_ = Dictionary->FreeList;
    }

    Result = Dictionary->FreeList;
    Dictionary->FreeList = Result->Next;
    Result->Next = 0;

    Assert(Result);
    return(Result);
}

static void
DictionaryInsertWord(struct dictionary *Dictionary, struct tst_node **RootNode, struct word_list_entry *Entry, u8 Index)
{
    if(*RootNode == 0)
    {
        *RootNode = DictionaryGetNewTSTNode(Dictionary);
        (*RootNode)->Character = Entry->Word[Index];
        (*RootNode)->Left = (*RootNode)->Equal = (*RootNode)->Right = 0;
        (*RootNode)->IsWord = false;
    }

    struct tst_node *Node = *RootNode;
    if(Entry->Word[Index] < Node->Character)
    {
        DictionaryInsertWord(Dictionary, &Node->Left, Entry, Index);
    }
    else if(Entry->Word[Index] > Node->Character)
    {
        DictionaryInsertWord(Dictionary, &Node->Right, Entry, Index);
    }
    else if(Entry->Word[Index] == Node->Character)
    {
        if(Entry->Word[Index + 1] == 0)
        {
            ++Dictionary->WordCount;
            Node->IsWord = true;
            Node->Entry = Entry;
        }
        else
        {
            DictionaryInsertWord(Dictionary, &Node->Equal, Entry, Index + 1);
        }
    }
    else
    {
        InvalidCodePath;
    }
}

static b32
DictionaryFindWord(struct tst_node *Root, char *Word, struct word_list_entry **FoundWordOut)
{
    b32 Result = false;

    if(Root == 0)
    {
        Result = false;
    }
    else if(Word[0] < Root->Character)
    {
        Result = DictionaryFindWord(Root->Left, Word, FoundWordOut);
    }
    else if(Word[0] > Root->Character)
    {
        Result = DictionaryFindWord(Root->Right, Word, FoundWordOut);
    }
    else if(Word[0] == Root->Character)
    {
        if(Word[1] == 0)
        {
            Result = Root->IsWord;
            if(Result && FoundWordOut)
            {
                *FoundWordOut = Root->Entry;
            }
        }
        else
        {
            Result = DictionaryFindWord(Root->Equal, Word + 1, FoundWordOut);
        }
    }
    else
    {
        InvalidCodePath;
    }

    return(Result);
}

static struct dictionary
DictionaryCreate(u32 InitialNodeCount, struct word_list_entry *WordList, u32 WordListSize)
{
    struct dictionary Result = {0};

    Result.Words = 0;
    Result.InitialNodeCount = InitialNodeCount;
    Result.WordCount = 0;
    Result.FreeList = 0;
    Result.Memory_ = 0;

    for(u32 WordIndex = 0; WordIndex < WordListSize; ++WordIndex)
    {
        DictionaryInsertWord(&Result, &Result.Words, WordList + WordIndex, 0);
    }

    return(Result);
}

static void
DictionaryDestroy(struct dictionary *Dictionary)
{
    if(Dictionary)
    {
        Dictionary->Words = 0;
        Dictionary->InitialNodeCount = 0;
        Dictionary->WordCount = 0;
        Dictionary->FreeList = 0;
        struct tst_node *Node = Dictionary->Memory_;
        while(Node)
        {
            struct tst_node *NextNode = Node->NextMemoryBlock_;
            free(Node);
            Node = NextNode;
        }
    }
}

static void
DictionaryInOrderPrint(struct tst_node *Root)
{
    if(!Root)
    {
        return;
    }

    DictionaryInOrderPrint(Root->Left);

    if(Root->IsWord)
    {
        printf("%s\n", Root->Entry->Word);
    }

    DictionaryInOrderPrint(Root->Equal);
    DictionaryInOrderPrint(Root->Right);
}

static void
DictionaryInOrderPrintBySize(struct tst_node *Root, u32 Length)
{
    if(!Root)
    {
        return;
    }

    DictionaryInOrderPrintBySize(Root->Left, Length);

    if(Root->IsWord)
    {
        if(Root->Entry->Length == Length)
        {
            printf("%s\n", Root->Entry->Word);
        }
    }

    DictionaryInOrderPrintBySize(Root->Equal, Length);
    DictionaryInOrderPrintBySize(Root->Right, Length);
}

static void
TestTST(struct dictionary *Dictionary)
{
    char *WordsToFind[] =
    {
        "kell",
        "kelli",
        "kellia",
        "age",
        "ago",
        "blawny",
        "blawn",
        "lolwut",
        "test",
        "kellian",
        "orling",
        "pro",
        "protest",
        "teaneck",
        "advocate",
        "advocating",
        "certify",
        "door",
        "doors",
        "doo",
        "lizemores",
        "lira",
    };

    for(u32 WordIndex = 0; WordIndex < ArrayCount(WordsToFind); ++WordIndex)
    {
        b32 Found = DictionaryFindWord(Dictionary->Words, WordsToFind[WordIndex], 0);
        printf("%-12s: %s\n",
               (Found ? "FOUND" : "NOT FOUND"),
               WordsToFind[WordIndex]);
    }
}
