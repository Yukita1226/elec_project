package main

import (
	"net/http"
	"sync"
	"time"

	"github.com/gin-gonic/gin"
)


func main(){

	r := gin.Default()

	r.Use(func(c * gin.Context){
		c.Writer.Header().Set("Access-Control-Allow-Origin", "http://localhost:5173")
		c.Writer.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
		c.Writer.Header().Set("Access-Control-Allow-Headers", "Content-Type")

		c.Next()
	})
}