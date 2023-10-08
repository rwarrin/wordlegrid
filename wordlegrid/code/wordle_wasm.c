#include "common.h"
#include "wordle_dictionary.h"
#include "word_list_common.h"
#include "wordle_grid.h"

#include "wordle_dictionary.c"
#include "wordle_grid.c"

static b32 GlobalAppInitialized = false;
static struct dictionary GlobalDictionary = {0};

static u32 GlobalResultBufferMaxLength = Megabytes(4);
static u32 GlobalResultBufferLength = 0;
static u8 *GlobalResultBuffer = {0};

void
Init()
{
    if(!GlobalAppInitialized)
    {
        GlobalDictionary = DictionaryCreate(128, large_word_list, ArrayCount(large_word_list));
        ZeroSize_(GlobalResultBuffer, GlobalResultBufferMaxLength);
        GlobalResultBufferLength = 0;
        GlobalAppInitialized = true;
    }
}

void
Reset()
{
    ZeroSize_(GlobalResultBuffer, GlobalResultBufferMaxLength);
    GlobalResultBufferLength = 0;
}

u8 *
GetWords(u32 Width, u32 Height, char *Letters, u32 LettersLength)
{
    u32 GridWidth = 3;
    u32 GridHeight = 3;
    u8 GridString[] = "irnilagec";
    struct adjacency_table Table = BuildAdjacencyTable(GridWidth, GridHeight, GridString, ArrayCount(GridString));

    struct dictionary FoundWordsDictionary = DictionaryCreate(128, 0, 0);

    FindWordsInGrid(&GlobalDictionary, &Table, &FoundWordsDictionary);

    GlobalResultBufferLength = 0;
    for(u32 Length = 3; Length < 7; ++Length)
    {
        GlobalResultBufferLength += printf("%d LETTER WORDS:\n", Length);
        DictionaryInOrderPrintBySize(FoundWordsDictionary.Words, Length);
    }

    DictionaryDestroy(&FoundWordsDictionary);

    return(GlobalResultBuffer);
}
