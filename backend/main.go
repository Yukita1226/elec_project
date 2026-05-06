package main

import (
	//"sync"
	"time"
	"ev/controller"
	"github.com/gin-gonic/gin"
)


func main(){

	r := gin.Default()

	r.GET("/Getsolar",controller.Getsolar)
	r.GET("/Getcar",controller.Getcar)
	r.GET("/Getev",controller.Getev)
	r.GET("/Getgrid",  controller.Getgrid)


	go func() {
		for {
		controller.Readmod()
        time.Sleep(1 * time.Second)
		}
	}()

	r.Run(":8080")
	
}

//http://localhost:8080/Getsolar


