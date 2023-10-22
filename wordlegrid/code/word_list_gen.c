#include "common.h"
#include "word_list_common.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void *
MapFileIntoMemory(char *FileName)
{
    void *Result = 0;

    HANDLE ReadFileHandle = CreateFileA(FileName, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if(ReadFileHandle != INVALID_HANDLE_VALUE)
    {
        HANDLE ReadFileMapHandle = CreateFileMappingA(ReadFileHandle, 0, PAGE_READWRITE, 0, 0, 0);
        if(ReadFileMapHandle != 0)
        {
            // NOTE(rick): FILE_MAP_COPY is copy on write and changes are never
            // written back to the original file!
            void *ReadFileBasePointer = MapViewOfFile(ReadFileMapHandle, FILE_MAP_COPY, 0, 0, 0);
            if(ReadFileBasePointer != 0)
            {
                Result = ReadFileBasePointer;
            }
            else
            {
                fprintf(stderr, "Failed to map view of file\n");
            }

            CloseHandle(ReadFileMapHandle);
        }
        else
        {
            fprintf(stderr, "Failed to create file mapping.\n");
        }

        CloseHandle(ReadFileHandle);
    }
    else
    {
        fprintf(stderr, "Failed to open file %s\n", FileName);
    }

    return(Result);
}

inline void
UnmapFileFromMemory(void *Base)
{
    if(Base)
    {
        b32 UnmapResult = UnmapViewOfFile(Base);
        if(!UnmapResult)
        {
            fprintf(stderr, "Something went wrong in UnampViewOfFile\n");
        }
    }
}

static u32
CountLinesAndNormalize(u8 *Buffer)
{
    u32 Result = 0;
    while(*Buffer)
    {
        if((*Buffer == '\n') || (*Buffer == 0))
        {
            ++Result;
        }
        else
        {
            *Buffer |= 0x20;
        }
        ++Buffer;
    }

    return(Result);
}

static struct word_list_entry *
ParseFileToArrayOfWordListEntries(u8 *Buffer, u32 *ArrayCountOut, u32 LineCount, u32 MinLength, u32 MaxLength)
{
    struct word_list_entry *ArrayBase = 0;

    u32 ArrayMemorySize = sizeof(*ArrayBase)*LineCount;
    ArrayBase = (struct word_list_entry *)VirtualAlloc(0, ArrayMemorySize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    Assert(ArrayBase);

    u32 ArrayIndex = 0;
    u8 *WordBegin = Buffer;
    u32 WordLength = 0;
    while(*Buffer)
    {
        if(*Buffer == '\n')
        {
            if((WordLength >= MinLength) && (WordLength <= MaxLength))
            {
                struct word_list_entry *Entry = (ArrayBase + ArrayIndex++);
                Entry->Word = WordBegin;
                Entry->Length = WordLength;
            }

            *Buffer++ = 0;
            WordBegin = Buffer;
            WordLength = 0;
        }
        ++Buffer;
        ++WordLength;
    }
    *ArrayCountOut = ArrayIndex;

    return(ArrayBase);
}

inline void
FreeLines(void *Memory)
{
    if(Memory)
    {
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}

struct formatted_file
{
    u32 TotalLineCount;
    u32 WordCount;
    struct word_list_entry *Lines;
    u8 *FileBasePointer;
};

inline struct formatted_file
GetFormattedFile(char *FileName, u32 MinLength, u32 MaxLength)
{
    struct formatted_file Result = {0};

    Result.FileBasePointer = MapFileIntoMemory(FileName);
    Result.TotalLineCount = CountLinesAndNormalize(Result.FileBasePointer);
    Result.Lines = ParseFileToArrayOfWordListEntries(Result.FileBasePointer, &Result.WordCount, Result.TotalLineCount, MinLength, MaxLength);

    return(Result);
}

inline void
CloseFormattedFile(struct formatted_file File)
{
    FreeLines(File.Lines);
    UnmapFileFromMemory(File.FileBasePointer);
    File.TotalLineCount = 0;
    File.WordCount = 0;
}

static void
FisherYatesShuffleLines(struct formatted_file *FormattedFile, u32 Seed)
{
    // NOTE(rick): Fisher-Yates Shuffle
    srand(Seed);
    for(u32 Index = FormattedFile->WordCount - 1;
        Index > 0;
        --Index)
    {
        u32 RandomIndex = rand() % (Index + 1);
        struct word_list_entry Temp = FormattedFile->Lines[Index];
        FormattedFile->Lines[Index] = FormattedFile->Lines[RandomIndex];
        FormattedFile->Lines[RandomIndex] = Temp;
    }
}

static void
WriteLinesToCHeader(struct formatted_file *FormattedFile, char *FileName, char *ObjectName)
{
    FILE *File = fopen(FileName, "wb");
    if(File)
    {
        fprintf(File, "static struct word_list_entry %s[] =\n", ObjectName);
        fprintf(File, "{\n");

        struct word_list_entry *Entry = FormattedFile->Lines;
        for(u32 Index = 0, WordCount = FormattedFile->WordCount; Index < WordCount; ++Index)
        {
            fprintf(File, "\t{\"%s\", %d},\n", Entry->Word, Entry->Length);
            ++Entry;
        }

        fprintf(File, "};");
        fclose(File);
    }
}

int main(int ArgCount, char **Args)
{
    if(ArgCount == 7)
    {
        u32 MinLength = atoi(Args[5]);
        u32 MaxLength = atoi(Args[6]);
        if(MaxLength == 0)
        {
            MinLength = 0;
            MaxLength = UINT_MAX;
        }
        struct formatted_file FormattedFile = GetFormattedFile(Args[1], MinLength, MaxLength);

        u32 Seed = atoi(Args[4]);
        if(Seed)
        {
            FisherYatesShuffleLines(&FormattedFile, Seed);
        }

        WriteLinesToCHeader(&FormattedFile, Args[2], Args[3]);

        CloseFormattedFile(FormattedFile);
    }
    else
    {
        printf("Usage: %s [input file] [output file] [table name] [seed] [min length] [max length]\n", Args[0]);
        printf("\n");
        printf("When [seed] is 0 then no shuffle will be applied.\n");
        printf("If [max length] is 0 then length requirements are ignored.\n");
    }

    return(0);
}
