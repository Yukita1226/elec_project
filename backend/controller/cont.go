package controller

import (
	"net/http"

	"ev/entity"
	"github.com/gin-gonic/gin"

	"math/rand/v2"
)

var solar =  entity.Solar{}
var	ev  = entity.Evstation{}
var car = entity.Car{}


func Makenum(){
	solar.Ptotal = rand.Float64()* 100
	solar.Ptransfer = rand.Float64()*10

	ev.Ptotal = rand.Float64()* 100
	ev.Ptransfer = rand.Float64()* 100

	car.Ptotal = rand.Float64()* 100
	car.Ptransfer = rand.Float64()* 100

}

func Getsolar(c * gin.Context){

	c.JSON(http.StatusOK,solar);
}

func Getcar(c * gin.Context){

	c.JSON(http.StatusOK,car);
}

func Getev(c * gin.Context){

	c.JSON(http.StatusOK,ev);
}



