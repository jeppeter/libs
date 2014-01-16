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
import "strconv"

// BoolType
type BoolType struct {
    option;
}

func (b *BoolType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*bool);
    *ptr, ok = def.(bool);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected bool as default value, not '%v'", def));
    }
    return nil;
}

func (b *BoolType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreTrue, StoreFalse:
        return true;
    }
    return false;
}

func (op *OptionParser) Bool(a  ... interface{}) *bool {
    dest := new(bool);
    op.BoolVar(dest, a);
    return dest;
}

func (op *OptionParser) BoolVar(dest *bool, a ... interface{}) {
    typ := new(BoolType);
    op.createOption(a, dest, typ, StoreTrue);
}

// StringType
type StringType struct {
    option;
}

func (s *StringType) parseArg(arg []string) (interface{}, os.Error) {
    return arg[0], nil;
}

func (s *StringType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*string);
    *ptr, ok = def.(string);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected string as default value, not '%v'", def));
    }
    return nil;
}

func (s *StringType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreConst:
        return true;
    case Store:
        if nargs == 1 {
            return true;
        }
    }
    return false
}

func (op *OptionParser) String(a ... interface{}) *string {
    dest := new(string);
    op.StringVar(dest, a);
    return dest;
}

func (op *OptionParser) StringVar(dest *string, a ... interface{}) {
    typ := new(StringType);
    op.createOption(a, dest, typ, Store);
}

// incrementable
type incrementable interface {
    increment(interface{});
}

// IntType
type IntType struct {
    option;
}

func (it *IntType) parseArg(arg []string) (interface{}, os.Error) {
    i, ok := strconv.Atoi(arg[0]);
    if ok != nil {
        return nil, os.NewError(fmt.Sprintf("'%s' is not an integer", arg[0]));
    }
    return i, nil;
}

func (it *IntType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*int);
    *ptr, ok = def.(int);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected integer as default value, not '%v'", def));
    }
    return nil;
}

func (it *IntType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreConst, Count:
        return true;
    case Store:
        if nargs == 1 {
            return true;
        }
    }
    return false;
}
func (it *IntType) increment(dest interface{}) {
    ptr := dest.(*int);
    *ptr++;
}

func (op *OptionParser) Int(a ... interface{}) *int {
    dest := new(int);
    op.IntVar(dest, a);
    return dest;
}

func (op *OptionParser) IntVar(dest *int, a ... interface{}) {
    typ := new(IntType);
    op.createOption(a, dest, typ, Store);
}

// CallbackType
type CallbackType struct {
    option;
    fn interface{};
}

func (cb *CallbackType) validAction(action *Action, nargs int) bool {
    // Each callback uses a different Action object, but they all have the
    // same function.
    return action == nil;
}

func (cb *CallbackType) hasArgs() bool {
    return cb.nargs > 0;
}

func (cb *CallbackType) performAction(arg []string) os.Error {
    fn := reflect.NewValue(cb.fn).(*reflect.FuncValue);
    fnType := fn.Type().(*reflect.FuncType);
    values := make([]reflect.Value, len(arg));
    for i := 0; i < len(arg); i++ {
        switch v := fnType.In(i).(type) {
        case *reflect.StringType:
            values[i] = reflect.NewValue(arg[i]);
        case *reflect.IntType:
            x, ok := strconv.Atoi(arg[i]);
            if ok != nil {
                return os.NewError(fmt.Sprintf("'%s' is not an integer", arg[i]));
            }
            values[i] = reflect.NewValue(x);
        }
    }
    fn.Call(values);
    return nil;
}

func (op *OptionParser) Callback(a ... interface{}) interface{} {
    typ := new(CallbackType);
    op.createOption(a, nil, typ, nil);
    return nil;
}

// array
type array interface {
    append(dest interface{}, val interface{});
}

// StringArrayType
type StringArrayType struct {
    option;
}

func (sa *StringArrayType) parseArg(arg []string) (interface{}, os.Error) {
    if len(arg) == 1 {
        return arg[0], nil;
    } else {
        return arg, nil;
    }
    return nil, os.NewError("StringArrayType.parseArg");
}

func (s *StringArrayType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*[]string);
    *ptr, ok = def.([]string);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected []string as default value, not '%v'", def));
    }
    return nil;
}

func (sa *StringArrayType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreConst:
        return true;
    case Store:
        if nargs > 1 {
            return true;
        }
    case Append:
        if nargs == 1 {
            return true;
        }
    }
    return false;
}

func (sa *StringArrayType) append(dest interface{}, val interface{}) {
    a := dest.(*[]string);
    appendString(a, val.(string));
}

func (op *OptionParser) StringArray(a ... interface{}) *[]string {
    dest := new([]string);
    *dest = make([]string, 0, 5);
    op.StringArrayVar(dest, a);
    return dest;
}

func (op *OptionParser) StringArrayVar(dest *[]string, a ... interface{}) {
    typ := new(StringArrayType);
    op.createOption(a, dest, typ, Append);
}

// IntArrayType
type IntArrayType struct {
    option;
}

func (ia *IntArrayType) parseArg(arg []string) (interface{}, os.Error) {
    if len(arg) == 1 {
        i, ok := strconv.Atoi(arg[0]);
        if ok != nil {
            return nil, os.NewError(fmt.Sprintf("'%s' is not an integer", arg[0]));
        }
        return i, nil;
    } else {
        ret := make([]int, len(arg));
        for i, str := range arg {
            x, ok := strconv.Atoi(str);
            if ok != nil {
                return nil, os.NewError(fmt.Sprintf("'%s' is not an integer", str));
            }
            ret[i] = x;
        }
        return ret, nil;
    }
    return nil, os.NewError("IntArrayType.parseArg");
}

func (s *IntArrayType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*[]int);
    *ptr, ok = def.([]int);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected []int as default value, not '%v'", def));
    }
    return nil;
}

func (ia *IntArrayType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreConst:
        return true;
    case Store:
        if nargs > 1 {
            return true;
        }
    case Append:
        if nargs == 1 {
            return true;
        }
    }
    return false;
}

func (ia *IntArrayType) append(dest interface{}, val interface{}) {
    i := dest.(*[]int);
    appendInt(i, val.(int));
}

func (op *OptionParser) IntArray(a ... interface{}) *[]int {
    dest := new([]int);
    *dest = make([]int, 0, 5);
    op.IntArrayVar(dest, a);
    return dest;
}

func (op *OptionParser) IntArrayVar(dest *[]int, a ... interface{}) {
    typ := new(IntArrayType);
    op.createOption(a, dest, typ, Append);
}

// StringArrayArray
type StringArrayArrayType struct {
    option;
}

func (sa *StringArrayArrayType) parseArg(arg []string) (interface{}, os.Error) {
    return arg, nil;
}

func (sa *StringArrayArrayType) storeDefault(dest, def interface{}) os.Error {
    var ok bool;
    ptr := dest.(*[][]string);
    *ptr, ok = def.([][]string);
    if !ok {
        return os.NewError(fmt.Sprintf("Expected [][]string as default value, not '%v'", def));
    }
    return nil;
}

func (sa *StringArrayArrayType) validAction(action *Action, nargs int) bool {
    switch action {
    case StoreConst:
        return true;
    case Append:
        if nargs > 1 {
            return true;
        }
    }
    return false;
}

func (sa *StringArrayArrayType) append(dest, val interface{}) {
    a := dest.(*[][]string);
    appendStringArray(a, val.([]string));
}

func (op *OptionParser) StringArrayArray(a ... interface{}) *[][]string {
    dest := new([][]string);
    *dest = make([][]string, 0, 5);
    op.StringArrayArrayVar(dest, a);
    return dest;
}

func (op *OptionParser) StringArrayArrayVar(dest *[][]string, a ... interface{}) {
    typ := new(StringArrayArrayType);
    op.createOption(a, dest, typ, Append);
}

// HelpType
type HelpType struct {
    option;
    op *OptionParser;
}

func (h *HelpType) validAction(action *Action, nargs int) bool {
    return action == helpAction;
}

func (h *HelpType) performAction(arg []string) os.Error {
    usage := h.op.Usage();
    fmt.Println(usage);
    os.Exit(0);
    return nil;
}

func (op *OptionParser) Help(a ... interface{}) interface{} {
    typ := new(HelpType);
    typ.op = op;
    op.createOption(a, nil, typ, helpAction);
    return nil;
}
