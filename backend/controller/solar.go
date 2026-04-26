package controller

import(
	"time"

	"ev/entity"
	"github.com/gin-gonic/gin"

	"math/rand/v2"
)

var solar =  entity.Solar{}



func MakenumSolar(){ // test

	mu.Lock()

	solar.Ptotal = rand.Float64()* 100
	solar.Ptransfer = rand.Float64()* 100

	mu.Unlock()

}

func Getsolar(c * gin.Context){

	co ,err := upg.Upgrade(c.Writer, c.Request, nil)

	if err != nil{
		return 
	}

	defer co.Close()

    for {

		mu.Lock()

		err:= co.WriteJSON(solar)

		mu.Unlock()

		if err != nil{
			return
		}

        time.Sleep(1 * time.Second)
    }
}