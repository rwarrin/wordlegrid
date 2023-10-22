REM Run this from the code directory
emcc wordle_wasm.c -sASSERTIONS -sALLOW_MEMORY_GROWTH -s EXPORTED_FUNCTIONS="['_Init', '_Reset', '_Solve', '_ChangeDictionary', '_main', '_malloc', '_free']" -s NO_EXIT_RUNTIME -s "EXPORTED_RUNTIME_METHODS=['ccall', 'allocate', 'intArrayFromString']" --shell-file wasm_shell.html -o Y:\build\wordlegrid.html -O1 -DWASM_BUILD=1
