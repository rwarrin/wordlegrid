#include "word_list.h"
#include "google_word_list.h"

static struct adjacency_table
BuildAdjacencyTable(u32 Width, u32 Height, u8 *GridString, u32 GridStringLength)
{
    struct adjacency_table Result = {0};
    u32 ListSizeTotal = Width*Height;
    Result.Width = ListSizeTotal;
    Result.Height = ListSizeTotal;
    Result.Data = (struct adjacency_node *)malloc(sizeof(*Result.Data)*(ListSizeTotal));
    Assert(Result.Data);

    u32 TableSizeTotal = ListSizeTotal*ListSizeTotal;
    Result.Links = (u8 *)malloc(sizeof(*Result.Links)*(TableSizeTotal));
    Assert(Result.Links);
    ZeroSize_(Result.Links, sizeof(*Result.Links)*(TableSizeTotal));

    for(u32 Index = 0; Index < ListSizeTotal; ++Index)
    {
        Assert(Index < GridStringLength);
        u8 Character = GridString[Index];

        struct adjacency_node *Node = Result.Data + Index;
        Node->Visited = false;
        Node->Valid = ( (Character >= 'a') && (Character <= 'z') );
        Node->Character = Character;
    }

    for(u32 ListIndex = 0; ListIndex < ListSizeTotal; ++ListIndex)
    {
        // NOTE(rick): Compute grid XY
        s32 GridY = ListIndex / Width;
        s32 GridX = ListIndex % Width;

        // NOTE(rick): Compute table XY
        s32 MinX = GridX - 1;
        s32 MaxX = GridX + 1;
        s32 MinY = GridY - 1;
        s32 MaxY = GridY + 1;
        for(s32 Y = MinY; Y <= MaxY; ++Y)
        {
            for(s32 X = MinX; X <= MaxX; ++X)
            {
                if( (X == GridX) && (Y == GridY) )
                {
                    continue;
                }

                if( (X >= 0) && (X < Width) && (Y >= 0) && (Y < Height) )
                {
                    u32 Base = ListIndex * Result.Width;
                    u32 Offset = (Y * Width) + X;
                    u32 TableIndex = Base + Offset;

                    // NOTE(rick): Insert link
                    u8 *TableItem = Result.Links + TableIndex;
                    *TableItem = true;
                }
            }
        }
    }

    return(Result);
}

static void
DestroyAdjacencyTable(struct adjacency_table *Table)
{
    if(Table)
    {
        Table->Width = 0;
        Table->Height = 0;
        free(Table->Data);
        free(Table->Links);
        Table->Data = 0;
        Table->Links = 0;
    }
}

static void
DepthFirstSearch(struct adjacency_table *Table, u8 *Buffer, u32 BufferMaxLength, u32 BufferLength, u32 TableRow,
                 struct dictionary *Dictionary, struct dictionary *FoundWordsDictionary)
{
    struct adjacency_node *ThisNode = Table->Data + TableRow;
    if(!ThisNode->Visited)
    {
        ThisNode->Visited = true;
        Buffer[BufferLength++] = ThisNode->Character;
        Buffer[BufferLength] = 0;

        struct word_list_entry *FoundWordListEntry = 0;
        b32 WordFound = DictionaryFindWord(Dictionary->Words, Buffer, &FoundWordListEntry);
        if(WordFound)
        {
            DictionaryInsertWord(FoundWordsDictionary, &FoundWordsDictionary->Words,
                                 FoundWordListEntry, 0);
        }

        for(u32 AdjacencyIndex = 0; AdjacencyIndex < Table->Width; ++AdjacencyIndex)
        {
            u8 *Visible = Table->Links + (TableRow * Table->Width) + AdjacencyIndex;
            if(*Visible)
            {
                struct adjacency_node *Node = Table->Data + AdjacencyIndex;
                if(Node->Valid && !Node->Visited)
                {
                    DepthFirstSearch(Table, Buffer, BufferMaxLength, BufferLength, AdjacencyIndex, Dictionary, FoundWordsDictionary);
                }
            }
        }
        Buffer[BufferLength--] = 0;
        ThisNode->Visited = false;
    }
}

