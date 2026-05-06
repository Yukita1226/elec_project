package controller

import (
	"ev/entity"
	"fmt"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/simonvetter/modbus"
)

var g = entity.Grid{}

var (
	maxCapacityAh   float32 = 50.0 
	currentChargeAh float32 = 50.0 
)

func Readmod() {

	ct, err := modbus.NewClient(&modbus.ClientConfiguration{
		URL:     "tcp://192.168.1.254:502",
		Timeout: 3*time.Second,
		
	})

	if err != nil {
		fmt.Println("err new client", err)
		return
	}

	err = ct.Open()
	ct.SetUnitId(1)
	if err != nil {
		fmt.Println("err open ct", err)
		return
	}

	defer ct.Close()

	v, err := ct.ReadFloat32(9001, modbus.HOLDING_REGISTER)
	if err != nil {
		fmt.Println("Voltage error:", err)
	} else {
		mu.Lock()
		g.Voltage = v
		mu.Unlock()
	}

	am, err := ct.ReadFloat32(9007, modbus.HOLDING_REGISTER)
	if err != nil {
		fmt.Println("Amp error:", err)
	} else {

		mu.Lock()
		g.Amm = am
		mu.Unlock()
	}

	wt, err := ct.ReadFloat32(9019, modbus.HOLDING_REGISTER)
	if err != nil {
		fmt.Println("Watt error:", err)
	} else {

		mu.Lock()
		g.Watt = wt
		mu.Unlock()
	}
}

func MakeBatt() {

	mu.Lock()
	defer mu.Unlock()

	ahMoved := g.Amm * (1.0 / 3600.0)
	currentChargeAh += ahMoved

	if currentChargeAh > maxCapacityAh {
		currentChargeAh = maxCapacityAh
	} else if currentChargeAh < 0 {
		currentChargeAh = 0
	}

	g.Batt = (currentChargeAh / maxCapacityAh) * 50
}

func Getgrid(ctx *gin.Context) {

	c, err := upg.Upgrade(ctx.Writer, ctx.Request, nil)
	if err != nil {
		return
	}
	
	defer c.Close()

	for {

		MakeBatt()
		
		mu.Lock()
		err := c.WriteJSON(g)
		mu.Unlock()

		if err != nil {
			return
		}

		time.Sleep(1 * time.Second)
	}

}
