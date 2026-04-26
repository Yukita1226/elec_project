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

	controller.MakenumCar()
	go func() {
		for {
			controller.MakenumCar()
			time.Sleep(5 * time.Second)
		}
	}()

		controller.MakenumEv()
	go func() {
		for {
			controller.MakenumEv()
			time.Sleep(5 * time.Second)
		}
	}()

		controller.MakenumSolar()
	go func() {
		for {
			controller.MakenumSolar()
			time.Sleep(5 * time.Second)
		}
	}()
	r.Run(":8080")
	
}

//http://localhost:8080/Getsolar


