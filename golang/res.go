package main

import (
	"fmt"
	"os"
	"regexp"
)

func main() {
	if len(os.Args[1:]) < 2 {
		s := fmt.Sprintf("%s regexp strings ...", os.Args[0])
		os.Stderr.WriteString(s)
		os.Exit(3)
	}

	/*now we should make the reg*/
	r := regexp.MustCompile(os.Args[1])
	for _, v := range os.Args[2:] {
		ss := r.FindAllString(v, -1)
		fmt.Printf("%s (%v)\n", v, ss)
		if len(ss) > 0 {
			fmt.Printf("%s regexp match ", os.Args[1])
			for _, sv := range ss {
				fmt.Printf("%s|", sv)
			}
			fmt.Printf("\n")
		}
	}
}
