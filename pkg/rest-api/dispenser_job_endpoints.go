package restapi

import (
	"fmt"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
	"github.com/vMaroon/SmartDispenserCounter/datatypes"
	"github.com/vMaroon/SmartDispenserCounter/queries"
)

const (
	base10 = 10
	qWord  = 64
)

// getDispenserJobRequests responds with a list of the dispenser-jobs available.
func (s *Server) getDispenserJobRequests(ctx *gin.Context) {
	query := &queries.GetDispenserJob{
		Sortable: &queries.Sortable{},
	}

	if err := ctx.ShouldBind(query); err != nil {
		ctx.IndentedJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		fmt.Printf("failed to bind object in getDispenserJobRequests - %v\n", err)

		return
	}

	dispenserJobs := s.dbClient.GetDispenserJobs(query.Sortable)
	ctx.IndentedJSON(http.StatusOK, dispenserJobs)
}

// getActiveDispenserJobRequest responds with the active job.
func (s *Server) getActiveDispenserJobRequest(ctx *gin.Context) {
	dispenserJob := s.dbClient.GetNextJob()
	if dispenserJob == nil {
		ctx.IndentedJSON(http.StatusNotFound, gin.H{"error": "no job found"})
		return
	}

	ctx.IndentedJSON(http.StatusOK, *dispenserJob)
}

// insertDispenserJob inserts a job to the database and sets it as active.
func (s *Server) insertDispenserJob(ctx *gin.Context) {
	dispenserJob := &datatypes.DispenserJob{}

	if err := ctx.ShouldBind(dispenserJob); err != nil {
		ctx.IndentedJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		fmt.Printf("failed to bind object in insertDispenserJob - %v\n", err)

		return
	}

	id := s.dbClient.InsertDispenserJob(dispenserJob)

	ctx.IndentedJSON(http.StatusCreated, gin.H{"message": fmt.Sprintf("%d", id)})
}

// deleteDispenserJob deletes a job by ID from the database.
func (s *Server) deleteDispenserJob(ctx *gin.Context) {
	if id, found := ctx.GetQuery("id"); found {
		number, err := strconv.ParseUint(id, base10, qWord)
		if err != nil {
			ctx.IndentedJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			fmt.Printf("failed to parse id in deleteDispenserJob - %v\n", err)
		}

		ctx.IndentedJSON(http.StatusOK, s.dbClient.DeleteDispenserJob(number))

		return
	}

	ctx.IndentedJSON(http.StatusBadRequest, gin.H{"message": "id missing"})
}
