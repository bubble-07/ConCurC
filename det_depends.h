#include <stdlib.h>
#include "libs/digraph.h"
#include "libs/dict.h"
#include "libs/set.h"
#include "det_file_depends.h"

//determine if two of the file_depends_reults came from the same file
static inline int file_depends_result_eq(file_depends_result one, file_depends_result two) {
    return path_eq(one.file, two.file);
}
static inline int hash_file_depends_result(file_depends_result in) {
    return hash_path(in.file);
}

DEFINE_GRAPH(file_depends_result)

DEFINE_CONSTRUCT_GRAPH(file_depends_result)

file_depends_result_graph load_file_and_depends(fileLoc* in);
