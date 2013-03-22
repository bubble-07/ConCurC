#include "det_depends.h"

file_depends_result_dynarray load_deps(file_depends_result in) {
    file_depends_result_dynarray result = file_depends_result_dynarray_make(4);
    path_dynarray filerefs = path_set_to_dynarray(in.filerefs);
    file_depends_result tmp;
    fileLoc* file;
    size_t i;
    for (i=0; i < filerefs.size; i++) {
        file = load_file(filerefs.begin[i]);
        tmp = det_file_deps(primorder(parse(lex(file))), in.main_path);
        result = file_depends_result_dynarray_add(result, tmp);
    }
    return result;
}

file_depends_result_graph load_file_and_depends(fileLoc* in) {
    file_depends_result firstfile = det_file_deps(primorder(parse(lex(in))), NULL);
    file_depends_result_graph result;
    result = construct_file_depends_result_graph(firstfile, &load_deps, 30);
    return result;
}

int main(int argc, const char* argv[]) {
    fileLoc* file = load_file(realpath(argv[1], NULL));
    load_file_and_depends(file);
    return 0;
}

