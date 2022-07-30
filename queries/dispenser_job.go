package queries

// GetDispenserJob presents the members of the GetDispenserJob URL query.
type GetDispenserJob struct {
	*Sortable
}

// PostDispenserJob presents the members of the POST dispenser-job request.
type PostDispenserJob struct {
	RequesterName string `form:"requesterName" json:"requesterName"`
	TypeACount    string `form:"typeACount" json:"typeACount"`
	TypeBCount    string `form:"typeBCount" json:"typeBCount"`
}
