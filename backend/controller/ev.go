package controller

import(
	"time"

	"ev/entity"
	"github.com/gin-gonic/gin"

	"math/rand/v2"
)

var	ev  = entity.Evstation{}


func MakenumEv(){ // test

	mu.Lock()

	ev.Ptotal = rand.Float64()* 100
	ev.Ptransfer = rand.Float64()* 100

	mu.Unlock()

}

func Getev(c * gin.Context){

	co ,err := upg.Upgrade(c.Writer, c.Request, nil)

	if err != nil{
		return 
	}

	defer co.Close()

    for {

		mu.Lock()

		err:= co.WriteJSON(ev)

		mu.Unlock()

		if err != nil{
			return
		}

        time.Sleep(1 * time.Second)
    }
}