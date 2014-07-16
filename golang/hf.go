package main

import (
	"net/http"
	"os"
)

func main() {
	d := "."
	if len(os.Args) > 1 {
		d = os.Args[1]
	}
	// To serve a directory on disk (/tmp) under an alternate URL
	// path (/tmpfiles/), use StripPrefix to modify the request
	// URL's path before the FileServer sees it:
	http.ListenAndServe(":8080", http.StripPrefix("/update", http.FileServer(http.Dir(d))))
}
