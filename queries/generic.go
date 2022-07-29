package queries

// Sortable can be extended to query structures to support sorting logic.
type Sortable struct {
	AscendingOrder bool `form:"ascendingOrder" json:"ascendingOrder"`
}
