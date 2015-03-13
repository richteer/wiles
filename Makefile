# Makefile borrowed from the iiag project, thanks to sjrct

SHELL  = bash
TARGET = fermat
CC     = clang
YACC   = bison
LEX    = flex

# Flags that are used regardless of compiliation options go here
CCFL := -c -g -Wall
LDFL := -Wall

SRCS := scope.c node.c tree.c

# Construct file lists
OBJS := $(addprefix build/obj/,$(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix build/dep/,$(patsubst %.c,%.d,$(SRCS)))
SRCS := $(addprefix src/,$(SRCS))

# All the make rules
.PHONY: all clean install

all: $(TARGET)

$(TARGET): src/parser.tab.c src/lexer.c $(OBJS) 
	$(CC) $^ $(LDFL) -o $(TARGET) -lfl -ly

src/parser.tab.c: src/parser.y
	$(YACC) $< --defines=src/parser.tab.h --output=$@ -v

src/lexer.c: src/lexer.l
	$(LEX) -d -o $@ $<

build/obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CCFL) $< -o $@

build/sobj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $< -o $@

clean:
	rm -rf build
	rm -f $(TARGET)
	rm -f src/parser.tab.* src/lexer.c


