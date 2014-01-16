include $(GOROOT)/src/Make.$(GOARCH)

TARG=optparse-go.googlecode.com/hg/optparse
GOFILES=\
	optparse.go\
	actions.go\
	option.go\
	types.go\
	util.go\
	help.go\

include $(GOROOT)/src/Make.pkg

opttest: package test.go
	$(GC) -o main.$(O) -I_obj test.go
	$(LD) -o opttest -L_obj main.$(O)
