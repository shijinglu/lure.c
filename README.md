[![Build Status](https://travis-ci.com/shijinglu/lure.c.svg?branch=master)](https://travis-ci.com/shijinglu/lure.c)

## Overview

This rule evaluates a string expression given a context. 

#### Quick start

```
/* Setup context (context is re-usable) */
ContextPtr ctx = NewContext();
setDoubleContext(ctx, "PI", 3.14);
setCustomContext(ctx, "APP_VERSION", "v3.2.1", "semver");

/* Evaluate a string expression against the above context. */
lure_eval(context, " (PI == 3.14) && (APP_VERSION <= 'v3.4.5') "); 

```
