package main

import (
	inmemory "github.com/vMaroon/SmartDispenserCounter/pkg/db/in-memory"
	restapi "github.com/vMaroon/SmartDispenserCounter/pkg/rest-api"
)

func main() {
	// in memory DB
	inMemoryDB := inmemory.NewInMemoryDB()
	// restAPI server
	restAPIServer := restapi.NewRESTAPIServer(inMemoryDB)
	// start server
	restAPIServer.Start()
}
