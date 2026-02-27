# Zvec Ruby

!!! note "Experimental Project"
    This gem is an experimental collaboration with Claude, exploring what it takes to wrap a C++ vector database for Ruby. It won't replace SQLite3 + sqlite-vec for lightweight use cases, and it certainly won't replace PostgreSQL + pgvector for production workloads. But for a narrow niche — embedding and searching a small number of short documents without standing up a database server — it's an interesting option to have in the toolbox.

<table>
<tr>
<td width="40%" align="center" valign="top">
<img src="assets/images/zvec.png" alt="Zvec" width="280"><br>
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
- <string><a href="http://madbomber.github.io/zvec-ruby">Full Documentation Website</a>
</td>
</tr>
</table>

## What is Zvec?

Zvec is an embedded vector database that stores, indexes, and queries high-dimensional vectors alongside scalar metadata. Unlike client-server databases, zvec runs in-process — your data lives on disk and loads directly into memory, with no network round-trips. The Ruby gem wraps the C++ library through [Rice](https://github.com/ruby-rice/rice) native bindings.

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
