# Zvec

> [!NOTE]
> This gem is an experimental collaboration with Claude, exploring what it takes to wrap a C++ vector database for Ruby. It won't replace SQLite3 + sqlite-vec for lightweight use cases, and it certainly won't replace PostgreSQL + pgvector for production workloads. But for a narrow niche — embedding and searching a small number of short documents (2-3 paragraphs each) without standing up a database server — it's an interesting option to have in the toolbox.

<br>
<table>
<tr>
<td width="40%" align="center" valign="top">
<img src="docs/assets/images/zvec.png" alt="Zvec" width="280"><br>
<em>Embedded vector search for Ruby</em>
</td>
<td width="60%" valign="top">
<strong>Key Features</strong><br>

- <strong>Vector Indexes</strong> - HNSW, IVF, and Flat with cosine, L2, and inner-product metrics<br>
- <strong>Dense & Sparse Vectors</strong> - FP16, FP32, FP64, INT4, INT8 support<br>
- <strong>Scalar Fields</strong> - STRING, BOOL, INT32, INT64, FLOAT, DOUBLE, and array variants<br>
- <strong>Full CRUD</strong> - Insert, upsert, update, delete, and fetch by primary key<br>
- <strong>Filtered Queries</strong> - Combine vector similarity with scalar predicates<br>
- <strong>On-Disk Persistence</strong> - Flush and mmap support<br>
- <strong>Error Hierarchy</strong> - Structured exceptions mapping zvec status codes<br>
- <strong>Native Performance</strong> - C++ engine via <a href="https://github.com/ruby-rice/rice">Rice</a> bindings<br>
- <strong>Full Documentation</strong> - <a href="http://madbomber.github.io/zvec-ruby">Full Documentation Website</a>
</td>
</tr>
</table>

Ruby bindings for Alibaba's [zvec](https://github.com/alibaba/zvec) C++ vector database library. Store, index, and query high-dimensional vectors alongside scalar metadata — all from Ruby. The native extension uses [Rice](https://github.com/ruby-rice/rice) (v4.11) for C++/Ruby interop and CMake for building. The zvec C++ source is included as a git submodule.

> [!CAUTION]
> I set `rake install` up to mirror (I hope) the kind of performance that you can expect from a `gem install zvec` which will compile the actual zvec source on the user's machine.  This will take several minutes.  It may not work on all platforms. I'm not sure that it will work at all because there are so many pieces and so many platforms.

## Prerequisites

- Ruby >= 3.2.0
- CMake >= 3.26
- A C++17 compiler (Clang or GCC)
- ICU4C: `brew install icu4c@78` (macOS — CMake expects it at `/opt/homebrew/opt/icu4c@78`)

## Installation

Add to your Gemfile:

```ruby
gem "zvec"
```

Then build the native extension:

```bash
bundle install
cd ext && cmake --preset macos-release && cmake --build build/macos-release && cd ..
```

For development/debug builds:

```bash
cd ext && cmake --preset macos-debug && cmake --build build/macos-debug && cd ..
```

## Quick Start

```ruby
require "zvec"
require "tmpdir"

# Define a schema with a primary key, metadata, and a vector field
pk        = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
title     = Zvec::FieldSchema.create("title", Zvec::DataType::STRING)
embedding = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
              dimension:    4,
              index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("demo", [pk, title, embedding])

Dir.mktmpdir("zvec") do |dir|
  col = Zvec::Collection.create_and_open(File.join(dir, "demo"), schema)

  # Build and insert a document
  doc = Zvec::Doc.new
  doc.pk = "doc1"
  doc.set_field("pk",        Zvec::DataType::STRING,      "doc1")
  doc.set_field("title",     Zvec::DataType::STRING,      "Hello Zvec")
  doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3, 0.4])
  col.insert([doc])
  col.flush

  # Query by vector similarity
  results = col.query_vector("embedding", [0.1, 0.2, 0.3, 0.4], top_k: 1)
  results.each do |d|
    h = d.to_h(col.schema)
    puts "#{h['title']}  (score: #{h['score']})"
  end
end
```

## Usage

### Schema Definition

Every collection requires a primary key field (STRING) and at least one vector field. Add scalar fields to store metadata alongside vectors.

```ruby
pk    = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
name  = Zvec::FieldSchema.create("name", Zvec::DataType::STRING)
year  = Zvec::FieldSchema.create("year", Zvec::DataType::INT32)
vec   = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
          dimension:    384,
          index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("my_collection", [pk, name, year, vec])
```

### Data Types

| Constant | Description |
|----------|-------------|
| `STRING` | UTF-8 string |
| `BOOL` | Boolean |
| `INT32`, `INT64` | Signed integers |
| `UINT32`, `UINT64` | Unsigned integers |
| `FLOAT`, `DOUBLE` | Floating point |
| `VECTOR_FP32` | Dense float32 vector |
| `VECTOR_FP16`, `VECTOR_FP64` | Dense float16/float64 vectors |
| `VECTOR_INT4`, `VECTOR_INT8`, `VECTOR_INT16` | Dense integer vectors |
| `SPARSE_VECTOR_FP16`, `SPARSE_VECTOR_FP32` | Sparse vectors |
| `ARRAY_*` | Array variants of scalar types |

### Index Types

```ruby
# HNSW (default, best for most use cases)
Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE, m: 50, ef_construction: 500)

# Flat (exact search, no approximation)
Zvec::FlatIndexParams.new(Zvec::MetricType::L2)

# IVF (inverted file index, good for large datasets)
Zvec::IVFIndexParams.new(Zvec::MetricType::IP, n_list: 1024)

# Invert (scalar field index for filtered queries)
Zvec::InvertIndexParams.new
```

### Metric Types

| Constant | Description |
|----------|-------------|
| `COSINE` | Cosine distance (0 = identical) |
| `L2` | Euclidean distance |
| `IP` | Inner product |

### Collection Lifecycle

```ruby
# Create a new collection on disk
col = Zvec::Collection.create_and_open("/path/to/collection", schema)

# Open an existing collection
col = Zvec::Collection.open("/path/to/collection")

# Block form — auto-flushes on exit
Zvec.open_collection("/path/to/collection") do |col|
  # work with col...
end

# Persist buffered writes to disk
col.flush

# Permanently delete from disk
col.destroy!
```

### Collection Options

```ruby
opts = Zvec::CollectionOptions.new
opts.read_only = true
opts.enable_mmap = true
opts.max_buffer_size = 4096

col = Zvec::Collection.open("/path/to/collection", options: opts)
```

### Inserting Documents

```ruby
doc = Zvec::Doc.new
doc.pk = "item1"
doc.set_field("pk",        Zvec::DataType::STRING,      "item1")
doc.set_field("title",     Zvec::DataType::STRING,      "Example")
doc.set_field("year",      Zvec::DataType::INT32,       2024)
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3, 0.4])

statuses = col.insert([doc])
statuses.each { |s| puts s }  # => "OK"

col.flush
```

The `upsert` and `update` methods work the same way:

```ruby
col.upsert([doc])   # insert or replace
col.update([doc])   # update existing
```

### Querying Vectors

Use `query_vector` for a convenient interface:

```ruby
results = col.query_vector("embedding", [0.1, 0.2, 0.3, 0.4], top_k: 5)

results.each do |doc|
  h = doc.to_h(col.schema)
  puts "#{h['pk']}: #{h['title']} (score: #{h['score']})"
end
```

For more control, build a `VectorQuery` directly:

```ruby
vq = Zvec::VectorQuery.new
vq.topk = 10
vq.field_name = "embedding"
vq.filter = "year > 2020"
vq.include_vector = false
vq.query_params = Zvec::HnswQueryParams.new(ef: 500)
vq.output_fields = ["title", "year"]

schema_field = col.schema.get_field("embedding")
vq.set_vector(schema_field, [0.1, 0.2, 0.3, 0.4])

results = col.query(vq)
```

### Fetching by Primary Key

```ruby
docs = col.fetch(["item1", "item2"])
docs.each do |pk, doc|
  h = doc.to_h(col.schema)
  puts "#{pk}: #{h['title']}"
end
```

### Deleting Documents

```ruby
statuses = col.delete(["item1", "item2"])
col.delete_by_filter("year < 2000")
col.flush
```

### Error Handling

Zvec maps C++ status codes to a Ruby exception hierarchy under `Zvec::Error`:

```ruby
begin
  col = Zvec::Collection.open("/nonexistent")
rescue Zvec::NotFoundError => e
  puts e.message
rescue Zvec::Error => e
  puts "Zvec error: #{e.message}"
end
```

| Exception | Status Code |
|-----------|-------------|
| `Zvec::NotFoundError` | `NOT_FOUND` |
| `Zvec::AlreadyExistsError` | `ALREADY_EXISTS` |
| `Zvec::InvalidArgumentError` | `INVALID_ARGUMENT` |
| `Zvec::PermissionDeniedError` | `PERMISSION_DENIED` |
| `Zvec::FailedPreconditionError` | `FAILED_PRECONDITION` |
| `Zvec::ResourceExhaustedError` | `RESOURCE_EXHAUSTED` |
| `Zvec::UnavailableError` | `UNAVAILABLE` |
| `Zvec::InternalError` | `INTERNAL_ERROR` |
| `Zvec::NotSupportedError` | `NOT_SUPPORTED` |

## Examples

Working examples are in the `examples/` directory:

- **[01_basic_usage.rb](examples/01_basic_usage.rb)** — Schema definition, insert, query, fetch, delete, and block-form collection management with hand-crafted vectors.
- **[02_semantic_search.rb](examples/02_semantic_search.rb)** — Semantic search over markdown documents using real 384-dim text embeddings from the `informers` gem (all-MiniLM-L6-v2 model).

Run them with:

```bash
bundle exec ruby examples/01_basic_usage.rb
bundle exec ruby examples/02_semantic_search.rb
```

## Development

After checking out the repo, initialize the zvec submodule and install dependencies:

```bash
git submodule update --init --recursive
bundle install
```

Build the native extension:

```bash
cd ext && cmake --preset macos-debug && cmake --build build/macos-debug && cd ..
```

Run the tests:

```bash
bundle exec rake test
```

Launch an interactive console:

```bash
bin/console
```

## License

The gem is available as open source under the terms of the [MIT License](LICENSE.txt).
