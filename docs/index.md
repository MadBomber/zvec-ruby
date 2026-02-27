# Zvec Ruby

Ruby bindings for Alibaba's [zvec](https://github.com/alibaba/zvec) C++ vector database library.

!!! note "Experimental Project"
    This gem is an experimental collaboration with Claude, exploring what it takes to wrap a C++ vector database for Ruby. It won't replace SQLite3 + sqlite-vec for lightweight use cases, and it certainly won't replace PostgreSQL + pgvector for production workloads. But for a narrow niche — embedding and searching a small number of short documents without standing up a database server — it's an interesting option to have in the toolbox.

## What is Zvec?

Zvec is an embedded vector database that stores, indexes, and queries high-dimensional vectors alongside scalar metadata. Unlike client-server databases, zvec runs in-process — your data lives on disk and loads directly into memory, with no network round-trips.

The Ruby gem wraps the C++ library through [Rice](https://github.com/ruby-rice/rice) native bindings, giving you direct access to:

- **HNSW, IVF, and Flat** vector indexes
- **Cosine, L2, and inner-product** distance metrics
- **Dense and sparse** vector types (FP16, FP32, FP64, INT4, INT8)
- **Scalar fields** for metadata: STRING, INT32, INT64, FLOAT, DOUBLE, BOOL, and array variants
- **Filtered queries** combining vector similarity with scalar predicates
- **On-disk persistence** with flush and mmap support

## When to Use Zvec

Zvec fits well when you need:

- A self-contained vector store with no external dependencies
- Fast similarity search over a small-to-medium corpus (hundreds to low thousands of documents)
- An embedded database for CLI tools, scripts, or single-process applications
- A lightweight alternative to spinning up a database server for prototyping

## Quick Example

```ruby
require "zvec"
require "tmpdir"

# Define schema
pk  = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
vec = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
        dimension: 4,
        index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("demo", [pk, vec])

Dir.mktmpdir("zvec") do |dir|
  col = Zvec::Collection.create_and_open(File.join(dir, "demo"), schema)

  doc = Zvec::Doc.new
  doc.pk = "item1"
  doc.set_field("pk",        Zvec::DataType::STRING,      "item1")
  doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3, 0.4])
  col.insert([doc])
  col.flush

  results = col.query_vector("embedding", [0.1, 0.2, 0.3, 0.4], top_k: 1)
  puts results.first.to_h(col.schema)
end
```

## Getting Started

Head to the [Prerequisites](getting-started/prerequisites.md) page to set up your build environment, then follow the [Installation](getting-started/installation.md) and [Quick Start](getting-started/quick-start.md) guides.
