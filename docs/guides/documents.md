# Documents

A `Zvec::Doc` is a typed key-value container that holds a single record's data. Documents carry a primary key, typed fields, a score (populated after queries), and an operator flag.

## Creating a Document

```ruby
doc = Zvec::Doc.new
doc.pk = "item1"
doc.set_field("pk",    Zvec::DataType::STRING, "item1")
doc.set_field("title", Zvec::DataType::STRING, "Example")
doc.set_field("year",  Zvec::DataType::INT32,  2024)
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3, 0.4])
```

!!! important
    Always set both `doc.pk` and the primary key field via `set_field`. The `pk` attribute is used by zvec internally for indexing, while `set_field` stores the value for retrieval.

## Setting Fields

The `set_field` method requires three arguments: field name, data type, and value.

```ruby
doc.set_field(name, data_type, value)
```

The data type must match the field's schema definition. Passing the wrong type results in a conversion error.

### Scalar Fields

```ruby
doc.set_field("name",   Zvec::DataType::STRING, "Alice")
doc.set_field("active", Zvec::DataType::BOOL,   true)
doc.set_field("age",    Zvec::DataType::INT32,   30)
doc.set_field("score",  Zvec::DataType::FLOAT,   0.95)
```

### Dense Vectors

Pass a Ruby array of numbers:

```ruby
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3, 0.4])
```

The array length must match the field's `dimension`.

### Sparse Vectors

Pass a Ruby hash mapping integer indices to float values:

```ruby
doc.set_field("sparse", Zvec::DataType::SPARSE_VECTOR_FP32, { 42 => 0.8, 99 => 0.3 })
```

### Array Fields

Pass a Ruby array of the appropriate scalar type:

```ruby
doc.set_field("tags", Zvec::DataType::ARRAY_STRING, ["ruby", "database", "vector"])
```

### Null Values

For nullable fields, pass `nil`:

```ruby
doc.set_field("bio", Zvec::DataType::STRING, nil)
```

## Reading Fields

Use `get_field` with the field name and data type:

```ruby
title = doc.get_field("title", Zvec::DataType::STRING)   # => "Example"
year  = doc.get_field("year",  Zvec::DataType::INT32)     # => 2024
vec   = doc.get_field("embedding", Zvec::DataType::VECTOR_FP32)  # => [0.1, 0.2, ...]
```

Returns `nil` if the field is absent or null.

## Alternative: set_field_by_schema

If you have a `FieldSchema` reference, you can use it for type dispatch instead of passing the data type explicitly:

```ruby
fs = schema.get_field("title")
doc.set_field_by_schema("title", fs, "New Title")
```

## Converting to a Hash

`Doc#to_h(schema)` converts all fields to a Ruby hash, using the schema for type dispatch:

```ruby
h = doc.to_h(col.schema)
# => {"pk"=>"item1", "score"=>0.0, "title"=>"Example", "year"=>2024, "embedding"=>[0.1, ...]}
```

The hash always includes `pk` and `score` keys.

## Document Metadata

```ruby
doc.pk         # => "item1" (primary key)
doc.score      # => 0.0 (distance, populated after query)
doc.doc_id     # => internal document ID
doc.operator   # => INSERT, UPSERT, UPDATE, or DELETE
```

## Inspecting Fields

```ruby
doc.field_names     # => ["pk", "title", "year", "embedding"]
doc.has_field?("title")  # => true
doc.has_value?("title")  # => true (has non-null value)
doc.null?("bio")         # => true (field is null)
doc.empty?               # => false (has at least one field)
```

## Modifying Fields

```ruby
doc.remove_field("year")   # remove a single field
doc.clear                  # remove all fields
```
