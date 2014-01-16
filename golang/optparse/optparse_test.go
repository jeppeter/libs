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
package optparse_test

import "os"
import "reflect"
import "testing"

import op "optparse"

func assertEqual(t *testing.T, a, b interface{}, msg string) {
    if !reflect.DeepEqual(a, b) {
        t.Errorf("%#v != %#v: %s", a, b, msg);
    }
}

func checkArgs(t *testing.T, args []string, err os.Error) {
    if len(args) > 0 {
        t.Error("Got args without passing any");
    }
    if err != nil {
        t.Fatalf("Received error from parsing: %s", err);
    }
}

func TestParser(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.Int("--aaa", "-aa", "-a");
    // having both -bb and -b would never be done in a real app...
    b := p.Int("--bbb", "-bb", "-b", op.Count);
    c := p.String("--ccc", "-cc", "-c");
    args, err := p.ParseArgs([]string{"--aaa=1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse --opt=arg");
    *a = 0;
    args, err = p.ParseArgs([]string{"--aaa", "1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse --opt arg");
    *a = 0;
    args, err = p.ParseArgs([]string{"-aa", "1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse -opt arg");
    *a = 0;
    args, err = p.ParseArgs([]string{"-aa=1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse -opt=arg");
    *a = 0;
    args, err = p.ParseArgs([]string{"-a1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse -oarg");
    *a = 0;
    args, err = p.ParseArgs([]string{"-a", "1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse -o arg");
    *a = 0;
    args, err = p.ParseArgs([]string{"-bbba1"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 1, "Failed to parse -pppoarg");
    assertEqual(t, *b, 3, "Failed to parse -ooo");
    *b = 0;
    args, err = p.ParseArgs([]string{"-bb"});
    checkArgs(t, args, err);
    assertEqual(t, *b, 1, "Failed to parse -opt");
    args, err = p.ParseArgs([]string{"-cFOO=BAR"});
    checkArgs(t, args, err);
    assertEqual(t, *c, "FOO=BAR", "Failed to parse -oFOO=BAR");

    g1 := op.OptionGroup(p, "SubGroup One", "Options specific to this subgruop");
    s1 := g1.Int("-s", "--subone");
    *b = 0;
    args,err = p.ParseArgs([]string{"-aa", "10", "-s", "11", "-bbb"})
    checkArgs(t, args, err);
    assertEqual(t, *a, 10, "Failed to set main level leading option");
    assertEqual(t, *s1, 11, "Failed to set sub-option");
    assertEqual(t, *b, 3, "Failed to set main level trailing option");
    g2 := op.OptionGroup(p, "SubGroup Two", "");
    s2 := g2.Int("-x");
    s3 := g2.Int("-y");
    g1_2 := op.OptionGroup(g1, "SubGroup One secondary subgroup", "nested subgroups")
    s4 := g1_2.Int("-z")
    *b = 0;
    args,err = p.ParseArgs([]string{"-aa", "20", "--subone=21", "-y", "24", "-b", "-b", "-z", "25", "-x", "23", "-b"})
    checkArgs(t, args, err);
    assertEqual(t, *a, 20, "Failed to set main level leading option");
    assertEqual(t, *s1, 21, "Failed to set sub-option");
    assertEqual(t, *b, 3, "Failed to set interim main level option");
    assertEqual(t, *s2, 23, "Failed to set second sublevel option");
    assertEqual(t, *s3, 24, "Failed to set second sublevel second option");
    assertEqual(t, *s4, 25, "Failed to set nested sublevel option");
}

func TestBool(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.Bool("-a");
    assertEqual(t, *a, false, "Bool defaulted to true");
    b := p.Bool("-b", op.StoreFalse);
    assertEqual(t, *b, true, "Bool with StoreFalse defaulted to false");
    args, err := p.ParseArgs([]string{"-a", "-b"});
    checkArgs(t, args, err);
    assertEqual(t, *a, true, "Bool did not store true");
    assertEqual(t, *b, false, "Bool with StoreFalse did not store false");
}

func TestString(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.String("-a");
    assertEqual(t, *a, "", "String did not default to empty string");
    b := p.String("-b", op.Const("foo"));
    assertEqual(t, *b, "", "String with Const did not default to empty string");
    args, err := p.ParseArgs([]string{"-a", "blah", "-b"});
    checkArgs(t, args, err);
    assertEqual(t, *a, "blah", "String did not get passed argument");
    assertEqual(t, *b, "foo", "String did not get Const value");
}

func TestInt(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.Int("-a");
    assertEqual(t, *a, 0, "Int did not default to 0");
    b := p.Int("-b", op.Count);
    assertEqual(t, *b, 0, "Int with Count did not default to 0");
    c := p.Int("-c", op.Const(10));
    assertEqual(t, *c, 0, "Int with Const did not default to 0");
    args, err := p.ParseArgs([]string{"-a", "20", "-bbb", "-c"});
    checkArgs(t, args, err);
    assertEqual(t, *a, 20, "Int did not get passed argument");
    assertEqual(t, *b, 3, "Int did not count correctly");
    assertEqual(t, *c, 10, "Int did not get Const value");
}

func TestCallback(t *testing.T) {
    p := op.NewParser("", 0);
    a := 0;
    callbackOne := func() { a = 1; };
    p.Callback("-a", callbackOne);
    b := 0;
    callbackTwo := func(i int) { b = i; };
    p.Callback("-b", callbackTwo);
    args, err := p.ParseArgs([]string{"-a", "-b", "1"});
    checkArgs(t, args, err);
    assertEqual(t, a, 1, "Callback did not fire");
    assertEqual(t, b, 1, "Callback with argument did not fire");
}

func TestStringArray(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.StringArray("-a");
    assertEqual(t, *a, []string{}, "StringArray did not default to empty array");
    b := p.StringArray("-b", op.Nargs(3), op.Store);
    assertEqual(t, *b, []string{}, "StringArray with Nargs did not default to empty array");
    c := p.StringArray("-c", op.Default([]string{"first"}))
    assertEqual(t, *c, []string{"first"}, "StringArray with default did not get correct default")
    args, err := p.ParseArgs([]string{"-a", "one", "-a", "two", "-b", "foo", "bar", "baz", "-c", "second"});
    checkArgs(t, args, err);
    assertEqual(t, *a, []string{"one", "two"}, "StringArray did not append successfully");
    assertEqual(t, *b, []string{"foo", "bar", "baz"}, "StringArray did not parse Nargs(3) successfully");
    assertEqual(t, *c, []string{"first", "second"}, "StringArray did not append to default successfully")
}

func TestIntArray(t *testing.T) {
    p := op.NewParser("", 0);
    a := p.IntArray("-a");
    assertEqual(t, *a, []int{}, "IntArray did not default to empty array");
    b := p.IntArray("-b", op.Nargs(3), op.Store);
    assertEqual(t, *b, []int{}, "IntArray with Nargs did not default to empty array");
    args, err := p.ParseArgs([]string{"-a", "1", "-a", "2", "-b", "5", "6", "7"});
    checkArgs(t, args, err);
    assertEqual(t, *a, []int{1, 2}, "IntArray did not append successfully");
    assertEqual(t, *b, []int{5, 6, 7}, "IntArray did not parse Nargs(3) successfully");
}
