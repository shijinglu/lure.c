
#########################
# customise these
GENFILES := lex.yy.c y.tab.c
CFILES := hashmap.c lure.c md5hash.c lure_extension.c
YACC_FLAGS := -g -v -t -y -d
CFLAGS := -g
LDFLAGS :=
########################

# -MMD generates dependencies while compiling
CFLAGS += -MMD
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

main:
	bison $(YACC_FLAGS) lure.y
	flex lure.l
	$(CC) $(GENFILES) $(CFILES) $(CFLAGS) lure_main.c -o lure.out

clean :
	rm -f $(PROG) $(OBJFILES) $(DEPFILES)


-include $(DEPFILES)#####
