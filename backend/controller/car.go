package controller

import(
	"time"

	"ev/entity"
	"github.com/gin-gonic/gin"

	"math/rand/v2"

)

var car = entity.Car{}

func MakenumCar(){ // test

	mu.Lock()

	car.Ptotal = rand.Float64()* 100
	car.Ptransfer = rand.Float64()* 100

	mu.Unlock()

}

func Getcar(c * gin.Context){

	co ,err := upg.Upgrade(c.Writer, c.Request, nil)

	if err != nil{
		return 
	}

	defer co.Close()

    for {

		mu.Lock()

		err:= co.WriteJSON(car)

		mu.Unlock()
		if err != nil{
			return
		}

        time.Sleep(1 * time.Second)
    }
}