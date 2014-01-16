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
import "path"
import "strings"
import "utf8"
import "syscall"
import "unsafe"

const _TIOCGWINSZ = 0x5413;
type winsize struct {
    ws_row, ws_col, ws_xpixel, ws_ypixel uint16;
}

func getColumns() (int, uintptr) {
    var w winsize;
    _, _, err := syscall.Syscall(syscall.SYS_IOCTL, 0, _TIOCGWINSZ, uintptr(unsafe.Pointer(&w)));
    return int(w.ws_col), err;
}

// Splits the string s over a number of lines width characters wide.
func linewrap(s string, width int, firstblank bool) []string {
    start := 0;
    length := -1;
    words := splitWords(s);
    lines := make([]string, 0, 5);
    if firstblank {
        appendString(&lines, "");
    }
    for i, word := range words {
        wordLen := utf8.RuneCountInString(word);
        if length + wordLen + 1 > width {
            appendString(&lines, strings.Join(words[start:i], " "));
            start = i;
            length = wordLen;
        } else {
            length += wordLen + 1;
        }
    }
    appendString(&lines, strings.Join(words[start:len(words)], " "));
    return lines;
}

func helpLines(opts []Option) []string {
    return nil;
}


func maxOptionColsize(op *OptionParser, max, limit int) int {
    for _, opt := range op.options {
        length := utf8.RuneCountInString(opt.String());
        if length > max && length < limit {
            max = length
        }
    }
    for _, sub := range op.optGroups {
        submax := maxOptionColsize(sub, max, limit);
        if submax > max {
            max = submax
        }
    }
    return max
}


func optionUsage(lines *[]string, op *OptionParser, format string, width, max_argcol int) {
    for _, opt := range op.options {
        optstr := opt.String();
        help := linewrap(opt.getHelp(), width, len(optstr) > max_argcol);
        appendString(lines, fmt.Sprintf(format, optstr, help[0]))
        for _, line := range help[1:len(help)] {
            appendString(lines, fmt.Sprintf(format, "", line));
        }
    }
    for grp, sub := range op.optGroups {
        appendString(lines, "");
        appendString(lines, fmt.Sprintf("%s Options:", grp));
        ulines := linewrap(sub.usage, width-2, false);
        for _, line := range ulines {
            appendString(lines, fmt.Sprintf("  %s", line))
        }
        optionUsage(lines, sub, format, width, max_argcol);
    }
}

func (op *OptionParser) Usage() string {

    //   |  -a, --arg    help information for arg      |
    //   |  -b           help information for b        |
    //   |  -l, --long-arg                             |
    //   |               help information for a long   |
    //   |               argument spanning multiple    |
    //   |               lines                         |
    //   ^--^---------^--^--------------------------^--^
    //    |     |     ||            |                | |
    //    |    max    |`-colsep   width              | `-COLUMNS
    //    indent      max_argcol                     `-gutter
    //
    //    COLUMNS    = read from env var of same name, defaulting to 80
    //                 if env var not set or setting is too small (min_width)
    //    min_argcol = minimum value of max

    const (
        indent = 2;
        colsep = 2;
        gutter = 1;
        min_argcol = 4;
        min_width = 5;
    )
    filler := indent + colsep + gutter;
    COLUMNS, err := getColumns();
    if err != 0 || COLUMNS < min_width {
        COLUMNS = 80;
    }
    max_argcol := COLUMNS / 3 - 2;
    if max_argcol < min_argcol {
        max_argcol = min_argcol;
    }

    //helps := make([][]string, len(op.options));
    //lines := make([]string, 0, 10);
    _, binName := path.Split(os.Args[0]);
    lines := []string {
        fmt.Sprintf("Usage: %s %s", binName, op.usage),
        "",
        "Options:",
    };
    max := maxOptionColsize(op, 0, max_argcol);
    width := COLUMNS - max - filler;
    if width < min_width {
        width = 55;
    }
    // What I wouldn't give for %*s specifiers...
    // %%%ds%%%%-%%ds%%%ds%%%%s
    // %2s%%-%ds%2s%%s
    //   %-24s  %s
    format := fmt.Sprintf(fmt.Sprintf("%%%ds%%%%-%%ds%%%ds%%%%s", indent, colsep),
                           " ", max, " ");
    optionUsage(&lines, op, format, width, max_argcol);
    return strings.Join(lines, "\n");
}
