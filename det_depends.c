#include "det_depends.h"

DEFINE_DICT(string, path)

string_path_dict glob_file_roots;
string_dynarray glob_backtable;


lexid depends_t(lexid root, lexid_tree_dynarray children) {
    if (isNonPrimID(root) && (children.size == 0)) {
        path p = string_path_dict_get(glob_file_roots, glob_backtable.begin[root.tokenval]);
        if (p != path_lookup_failure) {
            lexid result = FILEREF_LEXID;
            result.attr.stringval = path_to_string(p);
            return result;
        }
    }
    if (children.size == 1 && isNonPrimID(root)) {
        if (lexid_eq(children.begin[0].data, FILEREF_LEXID)) {
            path childpath = string_to_path(children.begin[0].data.attr.stringval);
            path rootpath = string_to_path(root.attr.stringval);
            DIR* dirp = opendir(childpath);
            if (dir_contains(dirp, rootpath)) {
                lexid result = FILEREF_LEXID;
                result.attr.stringval = path_to_string(cat_paths(childpath, rootpath));
                return result;
            }
        }
    }
    return root;
}


depends_result det_depends(parse_result in) {
    depends_result result;
    result.backsymtable = in.backsymtable;
    result.AST = in.AST;

    return result;
}
    

