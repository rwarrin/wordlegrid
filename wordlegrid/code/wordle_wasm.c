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

void Init(void)
{
    if(!GlobalAppInitialized)
    {
        GlobalDictionary = DictionaryCreate(128, large_word_list, ArrayCount(large_word_list));
        GlobalResultBuffer = (u8 *)malloc(sizeof(*GlobalResultBuffer)*GlobalResultBufferMaxLength);
        ZeroSize_(GlobalResultBuffer, GlobalResultBufferMaxLength);
        GlobalResultBufferLength = 0;
        GlobalAppInitialized = true;
    }
}

void
Reset(void)
{
    ZeroSize_(GlobalResultBuffer, GlobalResultBufferMaxLength);
    GlobalResultBufferLength = 0;
}

void
FindWordsInGrid_(struct dictionary *Dictionary, struct adjacency_table *Table,
                struct dictionary *FoundWordsDictionary)
{
    u8 StringBuilderBuffer[256] = {0};
    for(u32 ListIndex = 0; ListIndex < Table->Width; ++ListIndex)
    {
        DepthFirstSearch(Table, StringBuilderBuffer, ArrayCount(StringBuilderBuffer), 0, ListIndex, Dictionary, FoundWordsDictionary);
    }
}

void
Solve(u32 Width, u32 Height, char *Letters, u32 LettersLength)
{
    struct adjacency_table Table = BuildAdjacencyTable(Width, Height, Letters, LettersLength);

    struct dictionary FoundWordsDictionary = DictionaryCreate(128, 0, 0);

    FindWordsInGrid_(&GlobalDictionary, &Table, &FoundWordsDictionary);

    GlobalResultBufferLength = 0;
    for(u32 Length = 3; Length < 7; ++Length)
    {
        GlobalResultBufferLength += printf("%d LETTER WORDS:\n", Length);
        DictionaryInOrderPrintBySize(FoundWordsDictionary.Words, Length);
    }

    DictionaryDestroy(&FoundWordsDictionary);

    printf("%s\n", GlobalResultBuffer);
}

void
ChangeDictionary(u32 ID)
{
    if((ID >= 0) && (ID < 2))
    {
        DictionaryDestroy(&GlobalDictionary);
        switch(ID)
        {
            case 1:
            {
                GlobalDictionary = DictionaryCreate(128, google_word_list, ArrayCount(google_word_list));
            } break;
            case 0:
            default:
            {
                GlobalDictionary = DictionaryCreate(128, large_word_list, ArrayCount(large_word_list));
            } break;
        }
    }
}

int main(void)
{
    Init();
    return(0);
}
