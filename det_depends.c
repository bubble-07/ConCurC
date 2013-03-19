#include "det_depends.h"

DEFINE_DICT(string, path)

string string_lookup_failure = {0, 0, NULL};
DEFINE_REVERSE_LOOKUP(string, path)

string_path_dict glob_file_roots;
string_dynarray glob_backtable;


lexid depends_t(lexid root, lexid_tree_dynarray children) {
    if (isNonPrimID(root) && (children.size == 0)) {
        path p = string_path_dict_get(glob_file_roots, glob_backtable.begin[root.tokenval]);
        if (p != path_lookup_failure) {
            root.tokenval = FILEREF;
            root.attr.stringval = path_to_string(p);
        }
        return root;
    }
    lexid childid = NONE_LEXID;
    lexid rootid = NONE_LEXID;
    
    if (children.size == 2 && lexid_eq(root, EXPR_LEXID)) {
        printf("%s", "nooooo\n");
        childid = children.begin[1].data;
        rootid = children.begin[0].data;
    }
    else {
        return root;
    }
    /*fix the case where we "shadow" a sub-directory with one in the root set*/
    if (lexid_eq(rootid, FILEREF_LEXID)) {
        path rootpath = string_to_path(rootid.attr.stringval);
        string name = string_path_dict_reverse_get(glob_file_roots, rootpath);
        rootid.tokenval = 0;
        size_t i;
        for (i=0; i < glob_backtable.size; i++) {
            if (string_eq(name, glob_backtable.begin[i])) {
                rootid.tokenval = i;
            }
        }
        //TODO: THROW SOME KIND OF ERROR IF TOKENVAL=0
    }

    if (lexid_eq(childid, FILEREF_LEXID) && isNonPrimID(rootid)) {
        printf("%s", "hello\n");
        path childpath = string_to_path(childid.attr.stringval);
        path rootpath = string_to_path(glob_backtable.begin[rootid.tokenval]);
        printf("%s", childpath);
        printf("%s", "\n");
        printf("%s", rootpath);
        DIR* dirp = opendir(childpath);
        if (dir_contains(dirp, rootpath)) {
            root.tokenval = FILEREF;
            root.attr.stringval = path_to_string(cat_paths(childpath, rootpath));
        }
    }
    return root;
}

parse_result deps_test(parse_result in) {
    glob_backtable = in.backsymtable;
    glob_file_roots = string_path_dict_init(5);
    string testdir = to_dynstring("test");
    glob_file_roots = string_path_dict_add(glob_file_roots, string_path_bucket_make(testdir, 
                "/Users/bubble-07/Programmingstuff/test"));
    in.AST = lexid_tree_dfmap(in.AST, &depends_t);
    return in;
}



depends_result det_depends(parse_result in) {
    depends_result result;
    result.backsymtable = in.backsymtable;
    result.AST = in.AST;

    return result;
}
    

