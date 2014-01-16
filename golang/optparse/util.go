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

import "strings"
import "unicode"

func appendString(arr *[]string, s string) {
    if len(*arr) == cap(*arr) {
        tmp := make([]string, len(*arr), cap(*arr) * 2);
        for i, e := range *arr {
            tmp[i] = e;
        }
        *arr = tmp;
    }
    *arr = (*arr)[0:len(*arr)+1];
    (*arr)[len(*arr)-1] = s;
}

func appendInt(arr *[]int, x int) []int {
    if len(*arr) == cap(*arr) {
        tmp := make([]int, len(*arr), cap(*arr) * 2);
        for i, e := range *arr {
            tmp[i] = e;
        }
        *arr = tmp;
    }
    *arr = (*arr)[0:len(*arr)+1];
    (*arr)[len(*arr)-1] = x;
    return *arr
}

func appendStringArray(arr *[][]string, a []string) [][]string {
    if len(*arr) == cap(*arr) {
        tmp := make([][]string, len(*arr), cap(*arr) * 2);
        for i, e := range *arr {
            tmp[i] = e;
        }
        *arr = tmp;
    }
    *arr = (*arr)[0:len(*arr)+1];
    (*arr)[len(*arr)-1] = a;
    return *arr;
}

func splitWords(s string) []string {
    if s == "" {
        return make([]string, 0);
    }
    s = strings.TrimSpace(s);
    words := make([]string, 0, 5);
    start := 0;
    inWhitespace := false;
    for end, rune := range s {
        if unicode.IsSpace(rune) {
            if !inWhitespace {
                appendString(&words, s[start:end]);
            }
            inWhitespace = true;
        } else {
            if inWhitespace {
                start = end;
            }
            inWhitespace = false;
        }
    }
    appendString(&words, s[start:len(s)]);
    return words;
}
