# Quick Start

This guide walks through a complete workflow: defining a schema, creating a collection, inserting documents, querying by vector similarity, and cleaning up.

## 1. Define a Schema

Every collection needs at least a primary key field and one vector field. Scalar fields store metadata alongside vectors.

```ruby
require "zvec"
require "tmpdir"

pk    = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
title = Zvec::FieldSchema.create("title", Zvec::DataType::STRING)
year  = Zvec::FieldSchema.create("year", Zvec::DataType::INT32)

embedding = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
              dimension:    4,
              index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("movies", [pk, title, year, embedding])
```

## 2. Create a Collection

A collection is stored on disk at a specified path. Use `create_and_open` for new collections:

```ruby
col = Zvec::Collection.create_and_open("/tmp/my_movies", schema)
```

Or use a temporary directory for experiments:

```ruby
Dir.mktmpdir("zvec") do |dir|
  col = Zvec::Collection.create_and_open(File.join(dir, "movies"), schema)
  # ... work with col ...
end
```

## 3. Insert Documents

Build `Doc` objects with typed fields matching your schema:

```ruby
doc = Zvec::Doc.new
doc.pk = "mov1"
doc.set_field("pk",        Zvec::DataType::STRING,      "mov1")
doc.set_field("title",     Zvec::DataType::STRING,      "The Matrix")
doc.set_field("year",      Zvec::DataType::INT32,       1999)
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.9, 0.1, 0.0, 0.2])

statuses = col.insert([doc])
statuses.each { |s| puts s }  # => "OK"

col.flush  # persist to disk
```

## 4. Query by Vector Similarity

Find the most similar documents to a query vector:

```ruby
results = col.query_vector("embedding", [0.85, 0.2, 0.05, 0.15], top_k: 3)

results.each do |doc|
  h = doc.to_h(col.schema)
  puts "#{h['title']} (#{h['year']})  score=#{h['score']}"
end
```

!!! info "Score Interpretation"
    The `score` field contains the **distance** between vectors. For cosine distance, 0 means identical and values closer to 0 indicate higher similarity. To convert to a similarity value where higher is better, use `1.0 - score`.

## 5. Fetch by Primary Key

Retrieve specific documents without a vector search:

```ruby
docs = col.fetch(["mov1"])
docs.each do |pk, doc|
  h = doc.to_h(col.schema)
  puts "#{pk}: #{h['title']}"
end
```

## 6. Delete Documents

```ruby
col.delete(["mov1"])
col.flush
```

## 7. Block-Form Collections

`Zvec.open_collection` opens an existing collection, yields it, and auto-flushes on block exit:

```ruby
Zvec.open_collection("/tmp/my_movies") do |col|
  results = col.query_vector("embedding", [0.1, 0.2, 0.3, 0.4], top_k: 5)
  # ...
end
# col is flushed automatically here
```

## Next Steps

- [Schemas and Fields](../guides/schemas-and-fields.md) — Learn about all supported data types and field options.
- [Vector Search](../guides/vector-search.md) — Understand distance metrics, query parameters, and result scoring.
- [Examples](../examples/index.md) — See complete working examples.
