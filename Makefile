#
# Makefile
#   This makefile only works right now with gmake :(
#

SHELL	      = bash
CLIENT_TARGET = fermat
CC            = clang
YACC          = bison
LEX           = flex

# Flags that are used regardless of compiliation options go here
CLIENT_CCFL := -c -g -Wall
CLIENT_LDFL := -Wall

CLIENT_SRCS := scope.c node.c tree.c

# Construct file lists
CLIENT_OBJS := $(addprefix build/obj/,$(patsubst %.c,%.o,$(CLIENT_SRCS)))
CLIENT_DEPS := $(addprefix build/dep/,$(patsubst %.c,%.d,$(CLIENT_SRCS)))
CLIENT_SRCS := $(addprefix src/,$(CLIENT_SRCS))

# All the make rules
.PHONY: all clean install

all: $(CLIENT_TARGET)

$(CLIENT_TARGET): src/parser.tab.c src/lexer.c $(CLIENT_OBJS) 
	$(CC) $^ $(CLIENT_LDFL) -o $(CLIENT_TARGET)

src/parser.tab.c: src/parser.y
	$(YACC) $< --defines=src/parser.tab.h --output=$@ -v

src/lexer.c: src/lexer.l
	$(LEX) $< -d -o $@

build/obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CLIENT_CCFL) $< -o $@

build/sobj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $< -o $@

clean:
	rm -rf build
	rm -f $(CLIENT_TARGET)


