# Collections

A collection is a persistent, on-disk container for documents. It owns the schema, the vector indexes, and the forward storage for scalar fields.

## Creating a Collection

Use `create_and_open` to create a new collection at a file system path:

```ruby
col = Zvec::Collection.create_and_open("/path/to/my_collection", schema)
```

This creates the directory structure on disk and returns an open collection handle.

!!! warning
    Calling `create_and_open` on a path that already exists raises `Zvec::AlreadyExistsError`.

## Opening an Existing Collection

```ruby
col = Zvec::Collection.open("/path/to/my_collection")
```

Raises `Zvec::NotFoundError` if the path does not contain a valid collection.

## Block-Form Open

`Zvec.open_collection` yields the collection and auto-flushes when the block exits:

```ruby
Zvec.open_collection("/path/to/my_collection") do |col|
  results = col.query_vector("embedding", query_vec, top_k: 5)
  # ...
end
# col is flushed automatically here
```

## Collection Options

Pass a `CollectionOptions` object to control how a collection is opened:

```ruby
opts = Zvec::CollectionOptions.new
opts.read_only = true       # open in read-only mode
opts.enable_mmap = true     # memory-map data files
opts.max_buffer_size = 4096 # write buffer size

col = Zvec::Collection.open("/path/to/my_collection", options: opts)
```

## Writing Documents

### Insert

```ruby
statuses = col.insert([doc1, doc2, doc3])
statuses.each { |s| puts s.ok? ? "OK" : s.message }
```

Returns an array of `Status` objects, one per document.

### Upsert

Insert or replace existing documents by primary key:

```ruby
statuses = col.upsert([doc])
```

### Update

Update fields on existing documents:

```ruby
statuses = col.update([doc])
```

## Flushing

Writes are buffered in memory. Call `flush` to persist to disk:

```ruby
col.flush
```

The block-form `Zvec.open_collection` flushes automatically on block exit.

## Reading Documents

### Fetch by Primary Key

```ruby
docs = col.fetch(["pk1", "pk2", "pk3"])
docs.each do |pk, doc|
  h = doc.to_h(col.schema)
  puts "#{pk}: #{h['title']}"
end
```

Returns a hash mapping primary keys to `Doc` objects. Missing keys are omitted.

### Vector Query

See [Vector Search](vector-search.md) for the full query interface.

```ruby
results = col.query_vector("embedding", query_vec, top_k: 10)
```

## Deleting Documents

### Delete by Primary Key

```ruby
statuses = col.delete(["pk1", "pk2"])
col.flush
```

### Delete by Filter

```ruby
col.delete_by_filter("year < 2000")
col.flush
```

## Collection Metadata

```ruby
col.path     # => "/path/to/my_collection"
col.schema   # => CollectionSchema
col.stats    # => CollectionStats
col.options  # => CollectionOptions
```

### Collection Stats

```ruby
stats = col.stats
stats.doc_count            # => 42
stats.index_completeness   # => {"embedding" => 1.0}
```

## Schema Modification

### Add a Column

```ruby
new_field = Zvec::FieldSchema.create("category", Zvec::DataType::STRING)
col.add_column(new_field)
```

With a default expression:

```ruby
col.add_column(new_field, expression: "'unknown'")
```

### Drop a Column

```ruby
col.drop_column("category")
```

### Alter a Column

Rename a column or change its schema:

```ruby
col.alter_column("old_name", rename: "new_name")
```

## Index Management

### Create an Index

Add an index to an existing column:

```ruby
params = Zvec::InvertIndexParams.new
col.create_index("category", params)
```

### Drop an Index

```ruby
col.drop_index("category")
```

### Optimize

Compact and optimize indexes:

```ruby
col.optimize
```

## Destroying a Collection

Permanently delete the collection and all its data from disk:

```ruby
col.destroy!
```

!!! danger
    This is irreversible. All data is deleted.
