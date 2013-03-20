#include "primorder.h"
#include "libs/filesys.h"
#include "libs/dynstring.h"

#ifndef DET_DEPENDS_DEFINED
#define DET_DEPENDS_DEFINED

typedef struct {
    lexid_tree AST;
    string_dynarray backsymtable;
    string_dynarray filerefs;
} depends_result;


depends_result det_deps(parse_result in);

parse_result deps_test(parse_result in, const_path file);



#endif
