#include "primorder.h"
#include "libs/filesys.h"
#include "libs/dynstring.h"
#include "libs/set.h"

#ifndef DET_DEPENDS_DEFINED
#define DET_DEPENDS_DEFINED

DEFINE_DICT(string, path)

static string string_lookup_failure = {0, 0, NULL};
DEFINE_REVERSE_LOOKUP(string, path)
DEFINE_GET_ALL_VALS(string, path)
DEFINE_GET_ALL(string, path)

DEFINE_SET(path)

typedef struct {
    lexid_tree AST;
    string_dynarray backsymtable;
    path_set filerefs;
    path file;
} file_depends_result;

typedef struct {
    string_path_dict file_roots;//  dictionary of "Roots" for extra-file references
    string_dynarray backtable;//  backwards symbol table 
} depends_t_state;

DEFINE_STATEFUL_TREE_OPS(lexid, depends_t_state)
DEFINE_STATEFUL_TREE_OPS(lexid, path_set)

file_depends_result det_file_deps(parse_result in);

#endif
