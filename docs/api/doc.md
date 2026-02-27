# Doc

`Zvec::Doc` is a typed key-value container representing a single document (record) in a collection.

## Constructor

```ruby
doc = Zvec::Doc.new
```

Creates an empty document.

## Instance Methods

### Primary Key and Metadata

| Method | Returns | Description |
|--------|---------|-------------|
| `pk` | String | Primary key value |
| `pk=` | — | Set primary key |
| `score` | Float | Distance score (populated after queries) |
| `score=` | — | Set score |
| `doc_id` | Integer | Internal document ID |
| `doc_id=` | — | Set document ID |
| `operator` | `Operator` | Operation type (INSERT, UPSERT, UPDATE, DELETE) |
| `operator=` | — | Set operator |

### Setting Fields

#### `set_field(name, data_type, value)`

Set a field value with explicit type dispatch:

```ruby
doc.set_field("title", Zvec::DataType::STRING, "Example")
doc.set_field("year",  Zvec::DataType::INT32,  2024)
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2, 0.3])
doc.set_field("sparse",    Zvec::DataType::SPARSE_VECTOR_FP32, { 42 => 0.8 })
doc.set_field("tags",      Zvec::DataType::ARRAY_STRING, ["a", "b"])
doc.set_field("bio",       Zvec::DataType::STRING, nil)  # set to null
```

#### `set_field_by_schema(name, field_schema, value)`

Set a field using a `FieldSchema` for type dispatch:

```ruby
fs = schema.get_field("title")
doc.set_field_by_schema("title", fs, "Example")
```

### Reading Fields

#### `get_field(name, data_type)`

Get a field value with explicit type dispatch. Returns `nil` if the field is absent or null.

```ruby
title = doc.get_field("title", Zvec::DataType::STRING)
vec   = doc.get_field("embedding", Zvec::DataType::VECTOR_FP32)
```

### Conversion

#### `to_h(schema)`

Convert all fields to a Ruby hash using a `CollectionSchema` for type dispatch:

```ruby
h = doc.to_h(col.schema)
# => {"pk"=>"item1", "score"=>0.123, "title"=>"Example", "year"=>2024, ...}
```

The hash always includes `pk` and `score` keys.

### Field Inspection

| Method | Returns | Description |
|--------|---------|-------------|
| `field_names` | Array | Names of all set fields |
| `has_field?(name)` | Boolean | Whether the field exists |
| `has_value?(name)` | Boolean | Whether the field has a non-null value |
| `null?(name)` | Boolean | Whether the field is null |
| `empty?` | Boolean | Whether the document has no fields |

### Mutation

| Method | Description |
|--------|-------------|
| `remove_field(name)` | Remove a single field |
| `clear` | Remove all fields |

### Display

| Method | Returns | Description |
|--------|---------|-------------|
| `to_s` | String | Debug representation |
