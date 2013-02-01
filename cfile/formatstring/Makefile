EXE=testformat
OBJS=testformat.o format_string.o
GCC=gcc
CFLAGS= -Wall 
OFLAGS= -Wall
DEPFILES=testformat_def.h testformat_func.c testformat.cases
NUM_ARGS=6
all:$(DEPFILES) $(EXE)

$(EXE):$(OBJS)
	$(GCC) $(OFLAGS) -o $@ $(OBJS)


testformat_def.h:bprint.py
	python bprint.py $(NUM_ARGS) >$@

testformat_func.c:codeformat.py
	python codeformat.py $(NUM_ARGS) >$@

testformat.cases:formattestcase.py
	python formattestcase.py > $@

%.o:%.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJS) $(DEPFILES)
