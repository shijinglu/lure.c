//
//  Created by Shijing Lu on 11/3/18.
//  Copyright © 2018 OpenAB. All rights reserved.
//

#ifndef __LUURE_PUBLIC_H__
#define __LUURE_PUBLIC_H__

/** Main AST Node Type */
typedef struct Expr Expr;

/** Type for a list of nodes  */
typedef struct ExprList ExprList;

/** Type of key value pairs used as context */
typedef struct Context Context;

/***/
typedef struct ContextExtension ContextExtension;


/**
 * Set String Context, e.g. USER_NAME: "Alice"
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. USER_NAME.
 * @param val value of the context
 */
void setStringContext(Context* ctx, char *key, char *val);

/**
 * Set Integer Context, e.g. CITY_ID: 1
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. CITY_ID.
 * @param val value of the context
 */
void setIntContext(Context *ctx, char *key,  int val);

/**
 * Set Double Context, e.g. PVALUE: 0.5
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. PVALUE
 * @param val value of the context
 */
void setDoubleContext(Context *ctx, char *key,  double val);

/**
 * Set boolean Context, e.g. SWITCH_ON: true
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. SWITCH_ON.
 * @param val value of the context
 */
void setBoolContext(Context *ctx, char *key,  bool val);

/**
 * Set Custom Context, e.g. APP_VERSION: v3.2.1/semver
 * NOTE: this setter does not free space, make sure ctx is empty before calling this.
 * @param ctx context object to be set
 * @param key case sensitive context key, e.g. APP_VERSION.
 * @param val value of the context
 * @param typeDesc a string to find the right extension, e.g "semver"
 */
void setCustomContext(Context *ctx, char *key,  char *val, char *typeDesc);

/**
 * Compile string into AST tree, assuming there is only one expr.
 * @param s string to parse
 * @return AST root node
 */
Expr *compile(char *s);

/**
 * Evaluate expression with ctx, return false in case of error.
 * @param ctx a map of context objects
 * @param root the AST tree node.
 * */
bool eval(map_t ctx, Expr *root);

#endif /* __LUURE_PUBLIC_H__ */
