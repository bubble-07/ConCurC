#include "det_file_depends.h"

/* Transform the AST by replacing references to files [in identifiers] with FILEREF_LEXID's.
 * This transformation is intended to be carried out depth-first, so that folders can
 * be identified before the files they contain. The "state" parameter is needed to
 * determine the names of symbols and the potential root folders, as the AST doesn't
 * contain this information [wrapped in depends_t_state] */
lexid depends_t(lexid root, lexid_tree_dynarray children, depends_t_state state) {

    string_path_dict glob_file_roots = state.file_roots;
    string_dynarray glob_backtable = state.backtable;

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
            lexid sub_lexid = children.begin[0].data;
            if (lexid_eq(sub_lexid, FILEREF_LEXID)) {
                /*Special case handling shadowed identifier */
                string tmp = sub_lexid.attr.stringval;
                path tmppath = string_to_path(tmp);
                if (!path_is_rel(tmppath)) {
                    return root; //cannot actually have been shadowed
                }
                else {
                    //yup! it's been shadowed, all right!
                    size_t i = 0;
                    for (i=0; !string_eq(glob_backtable.begin[i], tmp); i++) {
                       if (i == glob_backtable.size) {
                           //THROW A BIG STINKIN' ERROR!
                           printf("%s", "ERRRRRRROOOORRRR \n");
                        }
                    }
                    sub_lexid.tokenval = i;
                }
                free(tmppath);
            }
            if (isNonPrimID(sub_lexid)) {
                path_dynarray top_paths;
                string top_string = top_lexid.attr.stringval;
                path testpath = string_to_path(top_string);

                if (path_is_rel(testpath)) {
                    //top path is actually relative -- resolve it
                    free(testpath);
                    top_paths = string_path_dict_get_all(glob_file_roots, top_string);
                }
                else {
                    //top path is not relative
                    top_paths = path_dynarray_make(1);
                    top_paths = path_dynarray_add(top_paths, testpath);
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

/* Since depends_t leaves unneeded children of FILEREFs, we need to remove them with a
 * breadth-first transformation. In addition, remove_unused_t adds paths to
 * refs to give a final set of external references */
lexid_tree_dynarray remove_unused_t(lexid_tree_dynarray children, lexid root, path_set refs) {

    if (lexid_eq(root, FILEREF_LEXID)) {
        lexid_tree_dynarray_recfree(children); //TODO: should really free lexid data.
        refs = path_set_add(refs, string_to_path(root.attr.stringval));
        //printf("%s", "\n here it is: ");
        //printf("%s", to_cstring(root.attr.stringval));
        //printf("%s", "\n");
        return lexid_tree_dynarray_make(1);
    }
    return children;
}

#define QUICKADD(name, filen) tmpstring = to_dynstring(name); \
    result = string_path_dict_add(result, string_path_bucket_make(tmpstring, filen));

/* Given a file's path, return a dictionary of accessible external file reference "roots".
 * If this is the first file examined, set main_path */
string_path_dict getroots(path file, path* main_path) {
    string tmpstring;
    string_path_dict result = string_path_dict_init(20); 

    QUICKADD("libs", "/usr/local/concur/libs")
    QUICKADD("test", "/Users/bubble-07/Programmingstuff/test")

    //main_path will be "stdin" if we are REPL'ing
    if (main_path != NULL) {
        if (*main_path == NULL) {
            if (file == NULL) {
                *main_path = "stdin";
            }
            else {
                *main_path = get_parent_dir(file);
            }
        }
    }
    else {
        //throw some kind of error...
        return result;
    }
        
    //If main_path is "stdin", we shouldn't have to add anything extra to the "libs" namespace
    if (*main_path != "stdin") {
        //but if it isn't, we should add the main_path
        QUICKADD( (get_innermost_dir(*main_path)), (*main_path))
    }
    path_dynarray stops = string_path_dict_get_all_values(result);

    path_dynarray roots = get_parent_dirs_to_root(file, stops);
    size_t i;
    for (i=0; i < roots.size; i++) {
        QUICKADD( (get_innermost_dir(roots.begin[i])) , (roots.begin[i]))
    }
    
    return result;
}
    

file_depends_result det_file_deps(parse_result in, path main_path) {
    file_depends_result result;
    result.backsymtable = in.backsymtable;
    result.AST = in.AST;

    depends_t_state state;

    path file = realpath(to_cstring(in.AST.data.loc.file), NULL);
    state.backtable = in.backsymtable;
    state.file_roots = getroots(file, &main_path);


    /*
    size_t i;
    size_t j;
    for (i=0; i < state.file_roots.size; i++) {
        for (j=0; j < state.file_roots.begin[i].size; j++) {
            printf("%s", to_cstring(state.file_roots.begin[i].begin[j].key));
            printf("%s", " ");
            printf("%s", state.file_roots.begin[i].begin[j].value);
            printf("%s", "\n");
        }
    } */

    path_set extern_refs = path_set_init(1);

    result.AST = lexid_tree_depends_t_state_dfmap(result.AST, &depends_t, state);
    result.AST = lexid_tree_path_set_hfmap(result.AST, &remove_unused_t, extern_refs);

    result.filerefs = extern_refs;
    result.file = realpath(to_cstring(in.AST.data.loc.file), NULL);
    result.main_path = main_path;

    return result;
}
