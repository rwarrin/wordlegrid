@echo off

SET CompilerFlags=-nologo -Z7 -Od -Oi -fp:fast -FC -MTd -Gm- -GR- -EHa-
SET LinkerFlags=-incremental:no -opt:ref

IF NOT EXIST build mkdir build
pushd build

REM Uncomment these lines to:
REM 1) Compile the word_list_gen program
REM 2) Run the word_list_gen program to generate the word_list.h header file for
REM the wordle grid program.
REM cl.exe %CompilerFlags% ../wordlegrid/code/word_list_gen.c /link %LinkerFlags%
REM Y:/build/word_list_gen.exe Y:/wordlegrid/data/words-95.txt Y:/wordlegrid/code/word_list.h "large_word_list" 32190347 3 9
REM Y:/build/word_list_gen.exe Y:/wordlegrid/data/googlewords.txt Y:/wordlegrid/code/google_word_list.h "google_word_list" 32190347 3 9

cl.exe %CompilerFlags% ../wordlegrid/code/main.c /link %LinkerFlags%
cl.exe %CompilerFlags% /c ../wordlegrid/code/wordle_wasm.c /link %LinkerFlags%

popd
