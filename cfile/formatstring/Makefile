EXE=testformat
OBJS=testformat.o format_string.o
GCC=gcc
CFLAGS= -Wall 
OFLAGS= -Wall
DEPFILES=testformat_def.h testformat_func.c
NUM_ARGS=3
all:$(DEPFILES) $(EXE)

$(EXE):$(OBJS)
	$(GCC) $(OFLAGS) -o $@ $(OBJS)


testformat_def.h:
	python bprint.py $(NUM_ARGS) >$@

testformat_func.c:
	python codeformat.py $(NUM_ARGS) >$@

%.o:%.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJS) $(DEPFILES)
