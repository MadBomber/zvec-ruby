# Basic Usage

**Source:** [`examples/01_basic_usage.rb`](https://github.com/MadBomber/zvec/blob/main/examples/01_basic_usage.rb)

This example demonstrates the core zvec workflow using hand-crafted 4-dimensional vectors.

## What It Covers

1. Defining a schema with scalar and vector fields
2. Creating a collection on disk
3. Inserting documents
4. Querying by vector similarity
5. Fetching documents by primary key
6. Deleting documents
7. Block-form `open_collection`

## Schema

The example creates a "movies" collection with four fields:

```ruby
pk_field    = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
title_field = Zvec::FieldSchema.create("title", Zvec::DataType::STRING)
year_field  = Zvec::FieldSchema.create("year", Zvec::DataType::INT32)
embedding   = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
                dimension:    4,
                index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("movies",
           [pk_field, title_field, year_field, embedding])
```

## Document Helper

A helper method builds a `Doc` with all fields populated:

```ruby
def make_doc(id, title, year, vector)
  doc = Zvec::Doc.new
  doc.pk = id
  doc.set_field("pk",        Zvec::DataType::STRING,      id)
  doc.set_field("title",     Zvec::DataType::STRING,      title)
  doc.set_field("year",      Zvec::DataType::INT32,       year)
  doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, vector)
  doc
end
```

## Sample Data

Five movies with hand-crafted 4D vectors, loosely representing genre characteristics:

```ruby
movies = [
  make_doc("mov1", "The Matrix",        1999, [0.9, 0.1, 0.0, 0.2]),
  make_doc("mov2", "Inception",          2010, [0.8, 0.3, 0.1, 0.1]),
  make_doc("mov3", "Interstellar",       2014, [0.7, 0.5, 0.3, 0.0]),
  make_doc("mov4", "The Grand Budapest", 2014, [0.1, 0.2, 0.9, 0.8]),
  make_doc("mov5", "Amélie",             2001, [0.0, 0.1, 0.8, 0.9]),
]
```

## Running

```bash
bundle exec ruby examples/01_basic_usage.rb
```

Expected output shows inserts, a similarity query, fetch by PK, deletion, and a block-form reopen.
