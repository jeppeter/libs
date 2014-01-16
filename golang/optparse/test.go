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
package main

import "fmt"
import "strings"

import op "optparse-go.googlecode.com/hg/optparse"

var s = "This is some sample text. Watermelon. This is some sample text. Watermelon. This is some sample text.\n Watermelon.     This is some sample text. Watermelon. This is some sample text. Watermelon."
var p = op.NewParser("[options] args...", op.EXIT_ON_ERROR | op.KEEP_UNKNOWN_OPTIONS)
var flag = p.Bool("--flag", "-t", op.Help(s))
var invert = p.Bool("--invert", "-T", op.StoreFalse)
var foo = p.String("--foo", "-f", op.Default("default"))
var i = p.Int("--int", "-i", op.Default(78))
var bar = p.StringArray("--bar", "-b", op.Default([]string{"one","two"}))
var c = p.Int("--count", "-c", op.Count)
var baz = p.StringArray("--baz", op.Store, op.Nargs(3), op.Metavar("A B C"))
var list = p.StringArrayArray("--list", op.Nargs(3), op.Metavar("X Y Z"))
var _ = p.Callback("--callback", func() { fmt.Println("Callback"); })
var _ = p.Callback("--callback-arg", "-a", func(i int, s string) {
    fmt.Printf("Callback: %d %s\n", i, s);
}, op.Help(s))
var _ = p.Help("-h", "--help")
var grp = op.OptionGroup(p, "subgroup", "Subgroup help information")
var sub1 = grp.Bool("-x", "--xflag", op.Help("subflag"))
var sub2 = grp.Int("-y", "--yint", op.Help("int subflag"))

func main() {
    args, _ := p.Parse()
    fmt.Printf("--flag=%t\n", *flag);
    fmt.Printf("--invert=%t\n", *invert);
    fmt.Printf("--foo=%s\n", *foo);
    fmt.Printf("--int=%d\n", *i);
    fmt.Printf("--bar=[%s]\n", strings.Join(*bar, ","));
    fmt.Printf("--count=%d\n", *c);
    fmt.Printf("--baz=[%s]\n", strings.Join(*baz, ","));
    fmt.Printf("--xflag=%t\n", *sub1);
    fmt.Printf("--yint=%d\n", *sub2);
    if len(*list) > 0 {
        fmt.Printf("--list=[\n");
        for i := 0; i < len(*list); i++ {
            fmt.Printf("  %v\n", (*list)[i]);
        }
        fmt.Printf("]\n");
    } else {
        fmt.Printf("--list=[]\n");
    }
    fmt.Printf("%v\n", args);
}
