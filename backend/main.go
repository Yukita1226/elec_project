package main

import (
	//"sync"
	"time"

	"ev/controller"

	"github.com/gin-gonic/gin"
)


func main(){

	r := gin.Default()
	//var mu sync.Mutex


	r.GET("/Getsolar",controller.Getsolar)
	r.GET("/Getcar",controller.Getcar)
	r.GET("/Getev",controller.Getev)

	controller.Makenum()
	go func() {
		for {
			controller.Makenum()
			time.Sleep(5 * time.Second)
		}
	}()
	r.Run(":8080")
	
}

//http://localhost:8080/Getsolar


