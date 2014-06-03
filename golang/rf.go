package main

import (
	"bufio"
	"fmt"
	//	"io"
	"log"
	"os"
	"path"
	"regexp"
	"strings"
)

func ReadFileExpress(rd *bufio.Reader, filter string) []string {

	var retstr []string
	re := regexp.MustCompile(filter)
	retstr = make([]string, 0)
	for {
		s, e := rd.ReadString('\n')
		if e != nil {
			break
		}
		s = strings.TrimRight(s, "\r\n")
		/*now we should match the express*/
		ms := re.FindAllString(s, -1)
		if len(ms) > 0 {
			//append(retstr, ms)

			log.Printf("ms = %v", ms)
			for _, v := range ms {
				retstr = append(retstr, v)
				fmt.Printf(" retstr = %v v = %v\n", retstr, v)
			}
		}

	}

	return retstr
}

func main() {

	if len(os.Args[1:]) < 2 {
		fmt.Fprintf(os.Stderr, "%s regularpatter files\n", path.Base(os.Args[1]))
		os.Exit(3)
	}
	log.SetFlags(log.Lshortfile)

	for _, v := range os.Args[2:] {
		inputfile, _ := os.Open(v)
		defer inputfile.Close()

		inputreader := bufio.NewReader(inputfile)

		regstr := ReadFileExpress(inputreader, os.Args[1])
		if len(regstr) > 0 {
			log.Printf("file %s has ", v)
			for _, vs := range regstr {
				log.Printf(",%v", vs)
			}
			log.Printf("\n")
		}
	}
	return
}
