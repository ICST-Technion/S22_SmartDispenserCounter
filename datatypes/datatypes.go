package datatypes

import (
	"github.com/vMaroon/SmartDispenserCounter/queries"
	"time"
)

// DispenserJob represents a dispenser-job entry.
type DispenserJob struct {
	ID                string    `json:"_id" form:"_id"`
	CreationTimestamp time.Time `json:"creationTimestamp" form:"creationTimestamp"`
	Status            bool      `json:"status" form:"status"`
	queries.PostDispenserJob
}
