#ifndef WORDLE_GRID_H

struct adjacency_node
{
    u8 Visited;
    u8 Valid;
    u8 Character;
};

struct adjacency_table
{
    u32 Width;
    u32 Height;
    struct adjacency_node *Data;
    u8 *Links;
};

#define WORDLE_GRID_H
#endif
