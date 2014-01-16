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
import "reflect"
import "strings"
import "utf8"

type _Help struct { x string; }
func Help(h string) *_Help { return &_Help{h} }
type _Nargs struct { x int; }
func Nargs(n int) *_Nargs { return &_Nargs{n} }
type _Argdesc struct { x string; }
func Metavar(a string) *_Argdesc { return &_Argdesc{a} }
type _Default struct { x interface{}; }
func Default(d interface{}) *_Default { return &_Default{d} }
type _Const struct { x interface{}; }
func Const(c interface{}) *_Const { return &_Const{c} }

type Option interface {
    getName() string;
    getNargs() int;
    hasArgs() bool;
    performAction([]string) os.Error;
    getHelp() string;
    setOpts([]string) os.Error;
    String() string;
    matches(string) bool;

    parseArg(arg []string) (interface{}, os.Error);
    storeDefault(dest, def interface{}) os.Error;
    validAction(action *Action, nargs int) bool;
    getOption() *option;
    getDest() interface{};
    getConst() interface{};
}

type option struct {
    typ Option;
    longOpts []string;
    shortOpts []string;
    dest interface{};
    help string;
    argdesc string;
    nargs int;
    action *Action;
    const_ interface{};
}

func destTypecheck(dest, value interface{}) bool {
    return reflect.Typeof(dest).(*reflect.PtrType).Elem() == reflect.Typeof(value);
}

func (op *OptionParser) createOption(args []interface{}, dest interface{}, typ Option, action *Action) Option {
    opts := make([]string, len(args));
    max := 0;
    opt := typ.getOption();
    opt.typ = typ;
    opt.dest = dest;
    for _, field:= range args {
        switch f := field.(type) {
        case string:
            opts[max] = f;
            max++;
        case *Action:
            action = f;
            if action == StoreFalse {
                err := typ.storeDefault(dest, true);
                if err != nil {
                    typ.setOpts(opts[0:max]);
                    op.ProgrammerError(typ.getName(), err.String());
                    return nil;
                }
            }
        case *_Help:
            opt.help = strings.TrimSpace(f.x);
        case *_Nargs:
            opt.nargs = f.x;
        case *_Argdesc:
            opt.argdesc = f.x;
        case *_Default:
            /*
            if !destTypecheck(dest, f.x) {
                ProgrammerError(fmt.Sprintf("%s: Type mismatch with default value.", opts[0]));
            }
            */
            err := typ.storeDefault(dest, f.x);
            if err != nil {
                typ.setOpts(opts[0:max]);
                op.ProgrammerError(typ.getName(), err.String());
                return nil;
            }
        case *_Const:
            opt.const_ = f.x;
            action = StoreConst;
        default:
            fnType, ok := reflect.Typeof(field).(*reflect.FuncType)
            if ok {
                opt.nargs = fnType.NumIn()
                typ.(*CallbackType).fn = f
            }
        }
    }
    opt.action = action;
    if max == 0 {
        op.ProgrammerError("<unknown>", "Option has no options!");
        return nil;
    }
    err := typ.setOpts(opts[0:max]);
    if err != nil {
        op.ProgrammerError("<unknown>", err.String());
        return nil;
    }
    name := typ.getName();
    if action == helpAction && opt.help == "" {
        opt.help = "Print this help message and exit.";
    }
    if opt.nargs == 0 && typ.hasArgs() {
        opt.nargs = 1;
    }
    if !typ.validAction(action, opt.nargs) {
        op.ProgrammerError(name, fmt.Sprintf("Option '%s' is using invalid action '%s'.", opts[0], action.name));
        return nil;
    }
    if opt.const_ != nil && !destTypecheck(dest, opt.const_) {
        op.ProgrammerError(name, fmt.Sprintf("%s: Type mismatch with constant value.", opts[0]));
        return nil;
    }
    if opt.argdesc == "" && opt.nargs > 0 {
        if len(opt.longOpts) > 0 {
            tmp := opt.longOpts[0];
            if strings.HasPrefix(tmp, "--") {
                opt.argdesc = tmp[2:len(tmp)];
            } else {
                opt.argdesc = tmp[1:len(tmp)];
            }
        } else {
            tmp := opt.shortOpts[0];
            opt.argdesc = tmp[1:len(tmp)];
        }
        opt.argdesc = strings.ToUpper(opt.argdesc);
        // strings.Replace would be nice...
        opt.argdesc = strings.Map(func(x int)int {
            if x == '-' {
                return '_';
            }
            return x;
        }, opt.argdesc);
    }
    op.appendOpt(typ);
    return typ;
}

func (o *option) getName() string {
    var ret string;
    if len(o.longOpts) > 0 {
        ret = o.longOpts[0];
    } else {
        ret = o.shortOpts[0];
    }
    if ret == "" {
        ret = "<unknown>";
    }
    return ret;
}

func (o *option) getOption() *option {
    return o;
}

func (o *option) getDest() interface{} {
    return o.dest;
}

func (o *option) getConst() interface{} {
    return o.const_;
}

func (o *option) parseArg(arg []string) (interface{}, os.Error) {
    return nil, nil;
}

func (o *option) storeDefault(dest, def interface{}) os.Error {
    return os.NewError("This option does not accept a default argument.");
}
func (o *option) validAction(action *Action, nargs int) bool {
    return false;
}

func (o *option) performAction(arg []string) os.Error {
    return o.action.fn(o.typ, arg);
}

func (o *option) hasArgs() bool {
    return o.action.hasArgs;
}

func (o *option) String() string {
    var ret string;
    if o.nargs == 0 {
        short := strings.Join(o.shortOpts, ", ");
        long := strings.Join(o.longOpts, ", ");
        if short != "" && long != "" {
            ret = short + ", " + long;
        } else if short != "" {
            ret = short;
        } else {
            ret = long;
        }
    } else {
        parts := make([]string, len(o.shortOpts) + len(o.longOpts));
        for i, opt := range o.shortOpts {
            parts[i] = opt + " " + o.argdesc;
        }
        for i, opt := range o.longOpts {
            parts[i + len(o.shortOpts)] = opt + " " + o.argdesc;
        }
        ret = strings.Join(parts, ", ");
    }
    return ret;
}

func (o *option) setOpts(opts []string) os.Error {
    i := len(opts);
    longOpts := make([]string, 0, i);
    shortOpts := make([]string, 0, i);
    for _, opt := range opts {
        j := utf8.RuneCountInString(opt);
        if strings.HasPrefix(opt, "-") {
            if j == 2 {
                shortOpts = shortOpts[0:len(shortOpts) + 1];
                shortOpts[len(shortOpts) - 1] = opt;
            } else {
                longOpts = longOpts[0:len(longOpts) + 1];
                longOpts[len(longOpts) - 1] = opt;
            }
        } else {
            return os.NewError(fmt.Sprintf("'%s' is not a valid option", opt));
        }
    }
    o.longOpts = longOpts;
    o.shortOpts = shortOpts;
    return nil;
}

func (o *option) getNargs() int {
    return o.nargs;
}

func (o *option) getHelp() string {
    return o.help;
}

func (o *option) matches(opt string) bool {
    j := utf8.RuneCountInString(opt);
    if j < 2 || opt[0] != '-' {
        return false;
    }
    if j == 2 {
        for _, s := range o.shortOpts {
            if s == opt {
                return true;
            }
        }
    } else {
        for _, s := range o.longOpts {
            if s == opt {
                return true;
            }
        }
    }
    return false;
}
