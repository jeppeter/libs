/*
Copyright 2009 Kirk McDonald

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
package optparse

import "fmt"
import "os"
import "strings"

type OptionParser struct {
    options []Option;
    optGroups map[string]*OptionParser;
    usage string;
    flags uint32;
}

const (
    NONE = 0;
    EXIT_ON_ERROR = 1 << iota;
    KEEP_UNKNOWN_OPTIONS;
)

func NewParser(usage string, flags uint32) *OptionParser {
    ret := new(OptionParser);
    ret.options = make([]Option, 0, 10);
    ret.optGroups = make(map[string]*OptionParser);
    ret.usage = usage;
    ret.flags = flags;
    return ret;
}

func OptionGroup(op *OptionParser, title string, desc string) *OptionParser {
    ret,ok := op.optGroups[title];
    if !ok {
        ret = new(OptionParser);
        ret.options = make([]Option, 0, 4);
        ret.optGroups = make(map[string]*OptionParser, 0);
        ret.usage = desc;
        ret.flags = op.flags;
        op.optGroups[title] = ret;
    }
    return ret
}

func (op *OptionParser) appendOpt(opt Option) {
    op.options = op.options[0:len(op.options)+1];
    op.options[len(op.options)-1] = opt;
    if len(op.options) == cap(op.options) {
        tmp := make([]Option, len(op.options), cap(op.options) * 2);
        for i, e := range op.options {
            tmp[i] = e;
        }
        op.options = tmp;
    }
}
func (op *OptionParser) matches(s string) Option {
    for _, option := range op.options {
        if option.matches(s) {
            return option;
        }
    }
    for _, optgrp := range op.optGroups {
        if sub := optgrp.matches(s); sub != nil {
            return sub;
        }
    }
    return nil;
}

func (op *OptionParser) printAndExit(err os.Error) {
    fmt.Fprintf(os.Stderr, "Error: %s\n%s\n", err, op.Usage());
    os.Exit(1);
}

func (op *OptionParser) optError(opt, msg string) os.Error {
    return os.NewError(fmt.Sprintf("%s: %s", opt, msg));
}
func (op *OptionParser) ProgrammerError(opt, msg string) {
    fmt.Fprintf(os.Stderr, "Programmer error: %s: %s\n", opt, msg);
    os.Exit(2);
}

func (op *OptionParser) Parse() ([]string, os.Error) {
    return op.ParseArgs(os.Args[1:len(os.Args)]);
}

func (op *OptionParser) doAction(option Option, arg string, hasArg bool, args []string, i *int, pos *[]string) (bool, os.Error) {
    var current []string;
    usedArg := false;
    if option == nil {
        if op.flags & KEEP_UNKNOWN_OPTIONS != 0 {
            appendString(pos, args[*i]);
            return hasArg, nil;
        } else {
            return false, os.NewError("invalid option");
        }
    }
    nargs := option.getNargs();
    if nargs > 0 {
        current = make([]string, nargs);
        j := 0;
        if hasArg {
            current[0] = arg;
            j = 1;
            usedArg = true;
        }
        for ; j < len(current); j++{
            (*i)++;
            if *i >= len(args) {
                return false, os.NewError("insufficient arguments for option");
            }
            current[j] = args[*i];
        }
    } else {
        current = nil;
    }
    err := option.performAction(current);
    if err != nil {
        return false, err;
    }
    return usedArg, nil;
}

func (op *OptionParser) ParseArgs(args []string) ([]string, os.Error) {
    positional_args := make([]string, 0, len(args));
    var arg string;
    var hasArg bool;
    var err os.Error;
    var opt string;
    var option Option;
    for i := 0; i < len(args); i++ {
        opt = args[i];
        idx := strings.Index(opt, "=");
        if opt == "--" {
            i++;
            for ; i < len(args); i++ {
                appendString(&positional_args, args[i])
            }
        } else if strings.HasPrefix(opt, "--") || strings.HasPrefix(opt, "-") && idx != -1 {
            if idx != -1 {
                arg = opt[idx + 1:len(opt)];
                hasArg = true;
                opt = opt[0:idx];
            } else {
                hasArg = false;
            }
            option = op.matches(opt);
            if option == nil && !strings.HasPrefix(opt, "--") {
                // for weird cases like -fFOO=BAR
                opt = args[i];
                goto parse_short_opt;
            }
            _, err = op.doAction(option, arg, hasArg, args, &i, &positional_args);
            if err != nil {
                goto error;
            }
        } else if strings.HasPrefix(opt, "-") {
            parse_short_opt:
            if option = op.matches(opt); option != nil && idx == -1{
                _, err = op.doAction(option, "", false, args, &i, &positional_args);
                if err != nil {
                    goto error;
                }
                continue;
            }
            for j, c := range opt[1:len(opt)] {
                s := "-" + string(c);
                if j == len(opt) - 2 {
                    hasArg = false;
                } else {
                    arg = opt[j + len(s):len(opt)];
                    hasArg = true;
                }
                var usedArg bool;
                option = op.matches(s);
                usedArg, err = op.doAction(option, arg, hasArg, args, &i, &positional_args);
                if err != nil {
                    goto error;
                }
                if usedArg {
                    break;
                }
            }
        } else {
            appendString(&positional_args, opt)
        }
    }
    return positional_args, nil;

    error:
    err = op.optError(opt, err.String());
    if op.flags & EXIT_ON_ERROR != 0 {
        op.printAndExit(err);
    }
    return nil, err;
}
