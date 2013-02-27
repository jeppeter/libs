EXE=testformat
OBJS=testformat.o format_string.o
GCC=gcc
CFLAGS= -Wall 
OFLAGS= -Wall
DEPFILES=testformat_def.h testformat_func.c
NUM_ARGS=9
all:$(DEPFILES) $(EXE) testcase.sh

$(EXE):$(OBJS)
	$(GCC) $(OFLAGS) -o $@ $(OBJS)


testformat_def.h:bprint.py
	python bprint.py $(NUM_ARGS) >$@

testformat_func.c:codeformat.py
	python codeformat.py $(NUM_ARGS) >$@


%.o:%.c
	$(GCC) $(CFLAGS) -c $< -o $@
testcase.sh:rtgen.py LocalException.py FormatGen.py
	python rtgen.py 100 ${NUM_ARGS}  >$@
	chmod u+x $@


clean:
	rm -f $(EXE) $(OBJS) $(DEPFILES) testcase.sh *.pyc
