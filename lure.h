//
//  Created by Shijing Lu on 11/3/18.
//  Copyright © 2018 OpenAB. All rights reserved.
//

#ifndef __LURE__H__
#define __LURE__H__
#include <stdbool.h>

/** Main AST Node Type */
typedef struct Node Node;

/** Type for a list of nodes  */
typedef struct NodeList NodeList;

/** Hashmap Type */
typedef void* ContextPtr;

/* Create a new context object. */
ContextPtr NewContext(void);

/**
 * Set String Context, e.g. USER_NAME: "Alice"
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. USER_NAME.
 * @param val value of the context
 */
void setStringContext(ContextPtr ctx, char *key, char *val);

/**
 * Set Integer Context, e.g. CITY_ID: 1
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. CITY_ID.
 * @param val value of the context
 */
void setIntContext(ContextPtr ctx, char *key,  int val);

/**
 * Set Double Context, e.g. PVALUE: 0.5
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. PVALUE
 * @param val value of the context
 */
void setDoubleContext(ContextPtr ctx, char *key,  double val);

/**
 * Set boolean Context, e.g. SWITCH_ON: true
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. SWITCH_ON.
 * @param val value of the context
 */
void setBoolContext(ContextPtr ctx, char *key,  bool val);

/**
 * Set Custom Context, e.g. APP_VERSION: v3.2.1/semver
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. APP_VERSION.
 * @param val value of the context
 * @param typeDesc a string to find the right extension, e.g "semver"
 */
void setCustomContext(ContextPtr ctx, char *key,  char *val, char *typeDesc);

/**
 * Compile string into AST tree, assuming there is only one expr.
 * Example: 
 * > Node *n = lure_compile("CITY_ID == 1");
 * > Data *d = n->evaluate(n, context);
 * > assert(d->toBoolean(d) == true);
 * > d->clean(); free(d);
 * > node_free(n); 
 * @param s string to parse
 * @return AST root node
 */
Node *lure_compile(const char *s);

/**
 * Evaluate expression with ctx, return false in case of error.
 * @param ctx a map of context objects
 * @param s string to eval.
 * */
bool lure_eval(ContextPtr ctx, const char *s);

#endif /* __LURE__H__ */
