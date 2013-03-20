#include "det_depends.h"
#include "libs/set.h"

DEFINE_DICT(string, path)

string string_lookup_failure = {0, 0, NULL};
DEFINE_REVERSE_LOOKUP(string, path)
DEFINE_GET_ALL_VALS(string, path)
DEFINE_GET_ALL(string, path)

DEFINE_SET(path)

string_path_dict glob_file_roots;
string_dynarray glob_backtable;
path_set glob_paths;
path main_path = NULL;


lexid depends_t(lexid root, lexid_tree_dynarray children) {
    if (isNonPrimID(root) && children.size == 0) {
        path test = string_path_dict_get(glob_file_roots, glob_backtable.begin[root.tokenval]);
        if (test != path_lookup_failure) {
            /* Must be in the list of roots, therefore, fileref*/
            root.attr.stringval = char_dynarray_copy(glob_backtable.begin[root.tokenval]);
            root.tokenval = FILEREF;
        }
    }
    else if (lexid_eq(root, EXPR_LEXID) && children.size == 2) {
        lexid top_lexid = children.begin[1].data;
        if (lexid_eq(top_lexid, FILEREF_LEXID)) {
            if (lexid_eq(children.begin[0].data, FILEREF_LEXID)) {
                /*Special case handling over-matched identifier */
            }
            else if (isNonPrimID(children.begin[0].data)) {
                lexid sub_lexid = children.begin[0].data;
                path_dynarray top_paths;
                string top_string = top_lexid.attr.stringval;

                if (path_is_rel(string_to_path(top_lexid.attr.stringval))) {
                    //top path is actually relative -- resolve it
                    top_paths = string_path_dict_get_all(glob_file_roots, top_string);
                }
                else {
                    //top path is not relative
                    top_paths = path_dynarray_make(1);
                    top_paths = path_dynarray_add(top_paths, string_to_path(top_string));
                }

                path sub_path = string_to_path(glob_backtable.begin[sub_lexid.tokenval]);
                path within = rel_get_within(sub_path, top_paths);
                if (!path_eq(within, path_lookup_failure)) {
                    path resulting = cat_paths(within, get_file_extn(sub_path, within));
                    root.attr.stringval = path_to_string(resulting);
                    root.tokenval = FILEREF;
                }
            }
        }
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

string_path_dict getroots(path file) {
    string tmpstring;
    string_path_dict result = string_path_dict_init(20); 

    QUICKADD("libs", "/usr/local/concur/libs")
    QUICKADD("test", "/Users/bubble-07/Programmingstuff/test")

    if (main_path == NULL) {
        main_path = get_parent_dir(file);
    }
    
    QUICKADD( (get_innermost_dir(main_path)), (main_path))
    path_dynarray stops = string_path_dict_get_all_values(result);

    path_dynarray roots = get_parent_dirs_to_root(file, stops);
    size_t i;
    for (i=0; i < roots.size; i++) {
        QUICKADD( (get_innermost_dir(roots.begin[i])) , (roots.begin[i]))
    }
    
    return result;
}
    

parse_result deps_test(parse_result in) {
    path file = realpath(to_cstring(in.AST.data.loc.file), NULL);
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
    

