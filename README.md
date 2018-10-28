## Overview

#### Quick start

```
/* compile string into expr, assuming there is only one expr. */
Expr *compile(char *s);

/* evaluate expression with ctx, return false in case of error. */
bool eval(map_t ctx, Expr *root);

/* build up contexts */
map_t ctx = hashmap_new();
hashmap_put(ctx, "USER_UUID", "abcd0123fedc");
hashmap_put(ctx, "CITY_ID", 1)

```

## Node Types

There are overall five types of node:

1. Literal, like "admin", 123, v3.2.1, true etc
2. Variables like CITY_ID, variables are string literals only that its key can be resolved from the context map.
    - in case of variables, save data into derived data field
3. Function node, like `md5mod(CITY_ID, 100)`
4. Expression list for instance (1, 2, 3), (admin, tester), (CITY_ID, 100) etc
5. Binary node like `CITY_ID == 1` or `CITY_ID in (1, 2, 3)`

NOTE: 
1. There is no unary expression node, because it can be converted into binary node, for example:
`NOT FLAG` is equivilent to `FALG != true`.
2. There is no teneray expression either, because it can be converted into two binary nodes, for example:
`0.05 <= pValue < 0.1` is equivilent to `(pValue >= 0.05) && (pValue <= 0.1)`


Five data types are supported:
1. bool. Any other types can be casted into bool // TODO: remember to convert `true` or `false` string listeral
2. int.
3. double.
4. string (char *).
5. generic (void *).

#### NOTE:

1. large integers will be converted to double: `currentTime >= 1424832200169` => `currentTime >= 1424832200169.0000000`
2. function name must be less than `MAX_IDENTIFIER_LENGTH`

# TODO:
[]. Add convinience methods for constructing contexts. like `addBoolCtx(char *key, bool val)`, etc.
[]. Split `lure.h` into `lure_internal.h` and `lure.h` or merge headers into one giant header.
