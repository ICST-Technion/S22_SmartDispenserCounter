package db

import (
	"github.com/vMaroon/SmartDispenserCounter/datatypes"
	"github.com/vMaroon/SmartDispenserCounter/queries"
)

// DispenserJobsHistoryLimit sets the history limit of dispenser jobs.
const DispenserJobsHistoryLimit = 50

// DB abstracts the functionality needed from a DB client.
type DB interface {
	DispenserJobHandler
}

// DispenserJobHandler abstracts the functionality needed from a DB client for handling dispenser-jobs.
type DispenserJobHandler interface {
	// GetDispenserJobs function to return jobs with sorting by timestamp.
	// This function is used to fetch history of up to -DispenserJobsHistoryLimit- latest jobs.
	GetDispenserJobs(sortable *queries.Sortable) []datatypes.DispenserJob
	// GetNextJob returns the job last inserted.
	GetNextJob() *datatypes.DispenserJob
	// InsertDispenserJob function to insert a job to the database.
	//
	// Returns the job's assigned ID.
	InsertDispenserJob(dispenserJob *datatypes.DispenserJob) uint64
	// DeleteDispenserJob function to delete a job given its unique identifier.
	// If a relevant job is found and is deleted, returns true. Otherwise, returns false.
	DeleteDispenserJob(jobID uint64) bool
	// Activate sets that the active (latest) entry in DB can be sent out.
	Activate()
	// Deactivate sets that the active (latest) entry in DB cannot be sent out.
	Deactivate()
}
