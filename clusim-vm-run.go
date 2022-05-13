package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"log"
	"bytes"
	"net/http"
	"github.com/gin-gonic/gin"
)

var (
	imageName    string
	verbose bool
)

func init() {
	flag.StringVar(&imageName, "i", "ubuntu-20.04", "specify image name")
	flag.BoolVar(&verbose, "v", false, "show verbose output")
	flag.Usage = usage
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage: vago-run {[image_name|ubuntu-20.04|opensuse-tumbleweed]} [-v]\n")
	fmt.Fprintf(os.Stderr, "\n")
	flag.PrintDefaults()
}

func main() {
	flag.Parse()

	if verbose {
		fmt.Printf("image name : %s\n", imageName)
	}

	r := gin.Default()

	gin.DebugPrintRouteFunc = func(httpMethod, absolutePath, handlerName string, nuHandlers int) {
			log.Printf("endpoint %v %v %v %v\n", httpMethod, absolutePath, handlerName, nuHandlers)
	}

	r.POST("/foo", func(c *gin.Context) {
		c.JSON(http.StatusOK, "foo")
	})

	r.GET("/bar", func(c *gin.Context) {
		c.JSON(http.StatusOK, "bar")
	})

	r.GET("/status", func(c *gin.Context) {
		c.JSON(http.StatusOK, "status")
	})

	os.Setenv("Path", os.Getenv("Path")+";C:\\Program Files\\qemu")
	fmt.Println("Path = " + os.Getenv("Path"))

	cmd := exec.Command("powershell", "qemu-system-x86_64 -smp 2 -m 2G -drive file=images/ubuntu-20.04.3.qcow2 -net user,hostfwd=tcp::10022-:22 -net nic -serial stdio")
	var out bytes.Buffer
	err := cmd.Run()
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("in all caps: %q\n", out.String())
	//r.Run()
}
