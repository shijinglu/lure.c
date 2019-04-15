
#########################
# customise these
YACC_FLAGS := -y -d
CFLAGS := -g
LDFLAGS :=
########################

# -MMD generates dependencies while compiling
CFLAGS += -MMD -Wall
CC := clang

.PHONY: clean test

pre-build:
	bison $(YACC_FLAGS) -y -d lure.y
	flex lure.l

test_util.out: pre-build util.c util.h util_test.c
	$(CC) util.c util_test.c -o test_util.out

test_data.out: pre-build data.h data_bool.c data_double.c data_int.c data_string.c data_version.c test_data.c util.c util.h
	$(CC) $(CFLAGS) data_bool.c data_double.c data_int.c data_string.c data_version.c test_data.c util.c -o test_data.out

test_node.out: pre-build data.h data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function.h function_md5mod.c util.c util.h \
	hashmap.c hashmap.h md5hash.c md5hash.h re.c re.h \
	node.c node.h node_binop.c node_function.c node_identity.c node_in.c node_like.c node_list.c node_literal.c \
	test_node.c
	
	$(CC) $(CFLAGS) data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function_md5mod.c hashmap.c md5hash.c node.c node_binop.c node_function.c node_identity.c node_in.c \
	util.c node_like.c node_list.c node_literal.c re.c test_node.c -o test_node.out

test_lure.out: pre-build ast_builder.c ast_builder.h data.h data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function.h function_md5mod.c hashmap.c hashmap.h lex.yy.c lure.c lure.h md5hash.c md5hash.h \
	node.c node.h node_binop.c node_function.c node_identity.c node_in.c node_like.c node_list.c node_literal.c \
	re.c re.h  util.c util.h y.tab.c y.tab.h test_lure.c

	$(CC) $(CFLAGS) ast_builder.c data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function_md5mod.c hashmap.c lex.yy.c lure.c md5hash.c util.c y.tab.c \
	node.c node_binop.c node_function.c node_identity.c node_in.c node_like.c node_list.c node_literal.c re.c \
	test_lure.c -o test_lure.out

test: test_util.out test_data.out test_node.out test_lure.out
	@echo "Running util tests"
	./test_util.out
	@echo "Running data tests"
	./test_data.out
	@echo "Running node tests"
	./test_node.out
	@echo "Running lure tests"
	./test_lure.out

clean :
	rm -f *.out
