# Filtered Queries

Zvec supports combining vector similarity search with scalar predicates. Filters narrow the candidate set before (or during) the vector search, returning only documents that match both the filter and the similarity criteria.

## Basic Filtering

Pass a `filter` string to `query_vector`:

```ruby
results = col.query_vector("embedding", query_vec,
            top_k: 5,
            filter: "year > 2020")
```

Or via `VectorQuery`:

```ruby
vq = Zvec::VectorQuery.new
vq.topk = 5
vq.field_name = "embedding"
vq.filter = "year > 2020 AND category = 'science'"
```

## Filter Syntax

Filters use a SQL-like expression language:

### Comparison Operators

| Operator | Example |
|----------|---------|
| `=` | `category = 'science'` |
| `!=` | `status != 'draft'` |
| `>` | `year > 2020` |
| `>=` | `score >= 0.5` |
| `<` | `price < 100` |
| `<=` | `count <= 10` |

### Logical Operators

| Operator | Example |
|----------|---------|
| `AND` | `year > 2020 AND category = 'science'` |
| `OR` | `category = 'science' OR category = 'tech'` |
| `NOT` | `NOT status = 'archived'` |

### String Matching

| Operator | Example | Description |
|----------|---------|-------------|
| `LIKE` | `title LIKE '%ruby%'` | Wildcard matching |

### Parentheses

Group expressions with parentheses for complex logic:

```ruby
filter: "(year > 2020 AND year < 2025) OR featured = true"
```

## Scalar Indexes

For best filter performance, add an inverted index to frequently filtered fields:

```ruby
# When defining the schema
category = Zvec::FieldSchema.create("category", Zvec::DataType::STRING,
             index_params: Zvec::InvertIndexParams.new)

# Or add to an existing collection
col.create_index("category", Zvec::InvertIndexParams.new)
```

Without an inverted index, filters still work but require a full scan of the forward storage.

## Delete by Filter

You can also use filter expressions to delete documents:

```ruby
col.delete_by_filter("year < 2000")
col.flush
```
