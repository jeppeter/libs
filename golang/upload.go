package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

// upload logic
func upload(w http.ResponseWriter, r *http.Request) {
	fmt.Println("method:", r.Method)
	if r.Method == "GET" {
		io.WriteString(w, `<html>
<head>
    <title>Upload file</title>
</head>
<body>
<form enctype="multipart/form-data" action="/upload" method="post">
      <input type="file" name="uploadfile" />
      <input type="hidden" name="token" value="{{.}}"/>
      <input type="submit" value="upload" />
</form>
</body>
</html>`)
	} else {
		r.ParseMultipartForm(32 << 20)
		file, handler, err := r.FormFile("uploadfile")
		if err != nil {
			fmt.Println(err)
			return
		}
		defer file.Close()
		//fmt.Fprintf(w, "%v", handler.Header)
		f, err := os.OpenFile("./test/"+handler.Filename, os.O_WRONLY|os.O_CREATE, 0666)
		if err != nil {
			fmt.Println(err)
			return
		}
		defer f.Close()
		io.Copy(f, file)
		http.Redirect(w, r, "/list", 301)
	}
}

func viewHandler(w http.ResponseWriter, r *http.Request) {
	imgid := r.FormValue("id")
	imgpath := "./test" + string(os.PathSeparator) + imgid
	if _, err := os.Stat(imgpath); os.IsNotExist(err) {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.Header().Set("Content-Type", "image")
	http.ServeFile(w, r, imgpath)
	return
}

func listHandler(w http.ResponseWriter, r *http.Request) {
	fileinfoArr, err := ioutil.ReadDir("./test")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	var listhtml string
	for _, fileinfo := range fileinfoArr {
		imgid := fileinfo.Name()
		listhtml += "<li><a href=\"/view?id=" + imgid + "\">" + imgid + "</li>"
	}

	io.WriteString(w, "<html><ol>"+listhtml+"</ol></html>")
	return
}

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "%s :port", os.Args[0])
		os.Exit(3)
	}
	fmt.Printf("listen on %s\n", os.Args[1])
	mux := http.NewServeMux()
	mux.HandleFunc("/upload", upload)
	mux.HandleFunc("/view", viewHandler)
	mux.HandleFunc("/list", listHandler)
	err := http.ListenAndServe(os.Args[1], mux)
	if err != nil {
		log.Fatal("ListenAndServe: ", err.Error())
	}
}
