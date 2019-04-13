
#########################
# customise these
GENFILES := lex.yy.c y.tab.c
CFILES := hashmap.c lure.c md5hash.c lure_extension.c
YACC_FLAGS := -g -v -t -y -d
CFLAGS := -g
LDFLAGS :=
########################

# -MMD generates dependencies while compiling
CFLAGS += -MMD -Wall
CC := gcc

lex.yy.c y.tab.h:
	bison -y -d lure.y

lex.yy.c:
	flex lure.l

test_parsing:
	bison $(YACC_FLAGS) lure.y
	flex lure.l
	$(CC) $(GENFILES) $(CFILES) $(CFLAGS) test_parsing.c -o test_parsing.out
	./test_parsing.out

test_eval:
	bison $(YACC_FLAGS) lure.y
	flex lure.l
	$(CC) $(GENFILES) $(CFILES) $(CFLAGS) test_eval.c -o test_eval.out
	./test_eval.out

test_util: util.c util.h util_test.c
	$(CC) util.c util_test.c -o test_util.out
	./test_util.out

test_data: data.h data_bool.c data_double.c data_int.c data_string.c data_version.c test_data.c util.c util.h
	$(CC) $(CFLAGS) data_bool.c data_double.c data_int.c data_string.c data_version.c test_data.c util.c -o test_data.out
	./test_data.out

test_node: data.h data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function.h function_md5mod.c util.c util.h \
	hashmap.c hashmap.h md5hash.c md5hash.h re.c re.h \
	node.c node.h node_binop.c node_function.c node_identity.c node_in.c node_like.c node_list.c node_literal.c \
	test_node.c
	
	$(CC) $(CFLAGS) data_bool.c data_double.c data_int.c data_string.c data_version.c \
	function.c function_md5mod.c hashmap.c md5hash.c node.c node_binop.c node_function.c node_identity.c node_in.c \
	util.c node_like.c node_list.c node_literal.c re.c test_node.c -o test_node.out
	
	./test_node.out

main:
	bison $(YACC_FLAGS) lure.y
	flex lure.l
	$(CC) $(GENFILES) $(CFILES) $(CFLAGS) lure_main.c -o lure.out

clean :
	rm -f $(PROG) $(OBJFILES) $(DEPFILES)


-include $(DEPFILES)#####
