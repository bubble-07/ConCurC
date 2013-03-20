#include "det_depends.h"
#include "libs/set.h"

DEFINE_DICT(string, path)

string string_lookup_failure = {0, 0, NULL};
DEFINE_REVERSE_LOOKUP(string, path)

DEFINE_SET(path)

string_path_dict glob_file_roots;
string_dynarray glob_backtable;
path_set glob_paths;


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
        path childpath = string_to_path(childid.attr.stringval);
        path rootpath = string_to_path(glob_backtable.begin[rootid.tokenval]);
        DIR* dirp = opendir(childpath);
        if (dir_contains(dirp, rootpath)) {
            root.tokenval = FILEREF;
            MUTATE(path, rootpath, get_file_extn(dirp, rootpath))
            path resultpath = cat_paths(childpath, rootpath);
            root.attr.stringval = path_to_string(resultpath);
        }
        //closedir(dirp); BE WARY OF THIS LINE! Somethin's wrong here!
    }
    return root;
}

lexid_tree_dynarray remove_unused_t(lexid_tree_dynarray children, lexid root) {
    if (lexid_eq(root, FILEREF_LEXID)) {
        lexid_tree_dynarray_free(children);
        glob_paths = path_set_add(glob_paths, string_to_path(root.attr.stringval));
        printf("%s", "\n here it is: ");
        printf("%s", to_cstring(root.attr.stringval));
        printf("%s", "\n");
        return lexid_tree_dynarray_make(1);
    }
    return children;
}

#define QUICKADD(name, filen) tmpstring = to_dynstring(name); \
    result = string_path_dict_add(result, string_path_bucket_make(tmpstring, filen));

string_path_dict getroots(const_path file) {
    string tmpstring;
    string_path_dict result = string_path_dict_init(20);
    QUICKADD("libs", "/usr/local/concur/libs")
    QUICKADD("test", "/Users/bubble-07/Programmingstuff/test")
    
    path_dynarray roots = get_parent_dirs_to_main(file);
    size_t i;
    for (i=0; i < roots.size; i++) {
        QUICKADD( (get_innermost_dir(roots.begin[i])) , (roots.begin[i]))
    }
    
    return result;
}
    

parse_result deps_test(parse_result in, const_path file) {
    glob_backtable = in.backsymtable;
    glob_paths = path_set_init(1);
    glob_file_roots = getroots(file);

    size_t i;
    size_t j;
    for (i=0; i < glob_file_roots.size; i++) {
        for (j=0; j < glob_file_roots.begin[i].size; j++) {
            printf("%s", to_cstring(glob_file_roots.begin[i].begin[j].key));
            printf("%s", " ");
            printf("%s", glob_file_roots.begin[i].begin[j].value);
            printf("%s", "\n");
        }
    }


    in.AST = lexid_tree_dfmap(in.AST, &depends_t);
    in.AST = lexid_tree_hfmap(in.AST, &remove_unused_t);
    return in;
}



depends_result det_depends(parse_result in) {
    depends_result result;
    result.backsymtable = in.backsymtable;
    result.AST = in.AST;

    return result;
}
    

