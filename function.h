
typedef struct {

    TypeInfo retType; //Return type of function
    cell_tree body; //Body of the function
    cell_dynarray params; //Parameters of the function
} function;
