package inmemory

// NewInMemoryDB returns a new instance of InMemoryDB.
func NewInMemoryDB() *DB {
	return &DB{
		dispenserJobDB: newDispenserJobDB(),
	}
}

// DB struct implements DB interface with in-memory logic.
type DB struct {
	*dispenserJobDB
}
