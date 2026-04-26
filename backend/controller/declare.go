package controller

import(
	"net/http"
	"sync"

	"github.com/gorilla/websocket"
)

var upg = websocket.Upgrader{

	CheckOrigin: func(r *http.Request) bool { return true },

}

var mu sync.Mutex