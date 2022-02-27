package main

import (
	"flag"
	"fmt"
	"os"
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
	//arg0, _ := strconv.Atoi(flag.Arg(0))
	if verbose {
		fmt.Printf("image name : %s\n", imageName)
	}
}
