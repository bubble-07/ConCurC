#include "parser.h"
#ifndef PRIMORDER_DEFINED
#define PRIMORDER_DEFINED
#include "parser.h"

/*Primitive reordering [DEF, LAMBDA, NAMESPACE ... etc] phase -- places the primitives in
the applicative position [first item in a list] */
parse_result primorder(parse_result in);

#endif
