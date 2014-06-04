package main

import (
	"fmt"
	"net"
	"os"
	"strconv"
)

func main() {
	if len(os.Args[1:]) < 1 {
		fmt.Fprintf(os.Stderr, "%s port\n", os.Args[0])
		os.Exit(3)
	}
	port, _ := strconv.Atoi(os.Args[1])
	conn := bind_server(port)
	if conn == nil {
		os.Exit(3)
	}

	defer conn.Close()

	server_handler(conn)
}

func server_handler(conn net.Listener) {

	for {
		accconn, err := conn.Accept()
		if err != nil {
			fmt.Fprintf(os.Stderr, "can not connect err %v\n", err)
			continue
		}

		go client_handler(accconn)
	}
}

func client_handler(conn net.Conn) {

	defer conn.Close()
	b := make([]byte, 1024)
	for {
		n, err := conn.Read(b)
		if err != nil || n == 0 {
			fmt.Fprintf(os.Stderr, "can not read bytes\n")
			break
		}

		conn.Write(b[:n])
	}
	return
}

func bind_server(port int) net.Listener {
	s := fmt.Sprintf(":%d", port)
	conn, err := net.Listen("tcp", s)
	if err != nil {
		fmt.Fprintf(os.Stderr, "bind %d port error %v\n", port, err)
		return nil
	}
	return conn
}
