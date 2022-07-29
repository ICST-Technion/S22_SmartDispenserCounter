package datatypes

import (
	"github.com/vMaroon/SmartDispenserCounter/queries"
	"time"
)

// DispenserJob represents a dispenser-job entry.
type DispenserJob struct {
	ID                uint64    `json:"id" form:"id"`
	CreationTimestamp time.Time `json:"creationTimestamp" form:"creationTimestamp"`
	queries.PostDispenserJob
}
