# Collection

`Zvec::Collection` is the primary interface for storing and querying documents. Collections are persistent on-disk databases that hold a schema, vector indexes, and forward storage.

!!! note "Shared Pointer Wrapper"
    Rice wraps `shared_ptr<Collection>` as `Std::SharedPtr<zvec::Collection>`. Ruby convenience methods (like `query_vector`) are mixed into this wrapper class automatically.

## Class Methods

### `Collection.create_and_open`

```ruby
Zvec::Collection.create_and_open(path, schema, options: nil)
```

Create a new collection on disk and return an open handle.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | String | File system path for the collection |
| `schema` | `CollectionSchema` | Schema defining the collection's fields |
| `options` | `CollectionOptions` | Optional open settings |

Raises `Zvec::AlreadyExistsError` if the path already exists.

### `Collection.open`

```ruby
Zvec::Collection.open(path, options: nil)
```

Open an existing collection.

Raises `Zvec::NotFoundError` if the path doesn't exist.

## Instance Methods

### Metadata

| Method | Returns | Description |
|--------|---------|-------------|
| `path` | String | Collection's file system path |
| `schema` | `CollectionSchema` | The collection's schema |
| `stats` | `CollectionStats` | Document count and index completeness |
| `options` | `CollectionOptions` | Current collection options |

### Write Operations

#### `insert(docs)`

```ruby
statuses = col.insert([doc1, doc2, doc3])
```

Insert new documents. Returns an array of `Status` objects.

#### `upsert(docs)`

```ruby
statuses = col.upsert([doc1, doc2])
```

Insert or replace documents by primary key.

#### `update(docs)`

```ruby
statuses = col.update([doc1])
```

Update existing documents.

#### `delete(pks)`

```ruby
statuses = col.delete(["pk1", "pk2"])
```

Delete documents by primary key. Returns an array of `Status` objects.

#### `delete_by_filter(filter)`

```ruby
col.delete_by_filter("year < 2000")
```

Delete all documents matching a filter expression. Raises on error.

### Read Operations

#### `fetch(pks)`

```ruby
docs = col.fetch(["pk1", "pk2", "pk3"])
```

Fetch documents by primary key. Returns a hash `{ pk_string => Doc }`. Missing keys are omitted.

#### `query(vector_query)`

```ruby
results = col.query(vq)
```

Execute a `VectorQuery`. Returns an array of `Doc` objects sorted by distance.

#### `query_vector` (convenience)

```ruby
results = col.query_vector(field_name, vector, top_k:, filter: nil,
            include_vector: false, query_params: nil, output_fields: nil)
```

Build and execute a `VectorQuery` in one call. See [VectorQuery](vector-query.md) for parameter details.

#### `group_by_query(group_query)`

```ruby
groups = col.group_by_query(gq)
```

Execute a `GroupByVectorQuery`. Returns an array of `GroupResult` objects.

### Lifecycle

#### `flush`

```ruby
col.flush
```

Persist buffered writes to disk.

#### `destroy!`

```ruby
col.destroy!
```

Permanently delete the collection and all data from disk.

### Schema Modification

#### `add_column(field_schema, expression: "", concurrency: 0)`

Add a new column to the collection.

#### `drop_column(name)`

Remove a column from the collection.

#### `alter_column(name, rename: "", new_schema: nil, concurrency: 0)`

Rename or modify a column.

### Index Management

#### `create_index(column, params, concurrency: 0)`

Create an index on an existing column.

#### `drop_index(column)`

Remove an index from a column.

#### `optimize(concurrency: 0)`

Compact and rebuild indexes.

## Module Method

### `Zvec.open_collection`

```ruby
Zvec.open_collection(path, options: nil) do |col|
  # work with col
end
# auto-flushed here
```

Block-form open that yields the collection and flushes on block exit.
