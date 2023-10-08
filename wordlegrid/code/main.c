#include "common.h"
#include "wordle_dictionary.h"
#include "word_list_common.h"
#include "wordle_grid.h"

#include "wordle_dictionary.c"
#include "wordle_grid.c"

static void
FindWordsInGrid(struct dictionary *Dictionary, struct adjacency_table *Table,
                struct dictionary *FoundWordsDictionary)
{
    u8 StringBuilderBuffer[256] = {0};
    for(u32 ListIndex = 0; ListIndex < Table->Width; ++ListIndex)
    {
        DepthFirstSearch(Table, StringBuilderBuffer, ArrayCount(StringBuilderBuffer), 0, ListIndex, Dictionary, FoundWordsDictionary);
    }
}

int main(void)
{
    u32 GridWidth = 3;
    u32 GridHeight = 3;
    u8 GridString[] = "r a ibcde";
    struct adjacency_table Table = BuildAdjacencyTable(GridWidth, GridHeight, GridString, ArrayCount(GridString));

    struct dictionary Dictionary = DictionaryCreate(128, large_word_list, ArrayCount(large_word_list));
    struct dictionary FoundWordsDictionary = DictionaryCreate(128, 0, 0);

    FindWordsInGrid(&Dictionary, &Table, &FoundWordsDictionary);
    for(u32 Length = 3; Length < 7; ++Length)
    {
        printf("\nLength %d\n", Length);
        DictionaryInOrderPrintBySize(FoundWordsDictionary.Words, Length);
    }

    DictionaryDestroy(&FoundWordsDictionary);
    DictionaryDestroy(&Dictionary);

    DestroyAdjacencyTable(&Table);

    return(0);
}
