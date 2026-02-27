# Schemas and Fields

Every zvec collection is defined by a schema that declares its fields, data types, and indexes.

## Creating a Field

Use `Zvec::FieldSchema.create` to define a field:

```ruby
field = Zvec::FieldSchema.create(name, data_type, dimension: 0, nullable: false, index_params: nil)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | String | Field name (must be unique within the schema) |
| `data_type` | `Zvec::DataType` | The type of data stored in this field |
| `dimension` | Integer | Vector dimensionality (required for vector fields, ignored for scalars) |
| `nullable` | Boolean | Whether the field can contain `nil` values (default: `false`) |
| `index_params` | IndexParams | Index configuration (required for vector fields) |

## Scalar Data Types

| Constant | Ruby Type | Description |
|----------|-----------|-------------|
| `DataType::STRING` | String | UTF-8 text |
| `DataType::BINARY` | String | Raw binary data |
| `DataType::BOOL` | Boolean | `true` or `false` |
| `DataType::INT32` | Integer | 32-bit signed integer |
| `DataType::INT64` | Integer | 64-bit signed integer |
| `DataType::UINT32` | Integer | 32-bit unsigned integer |
| `DataType::UINT64` | Integer | 64-bit unsigned integer |
| `DataType::FLOAT` | Float | 32-bit floating point |
| `DataType::DOUBLE` | Float | 64-bit floating point |

## Dense Vector Types

Dense vectors are stored as fixed-length arrays. You must specify a `dimension` and `index_params` when creating a vector field.

| Constant | Element Type | Use Case |
|----------|-------------|----------|
| `DataType::VECTOR_FP32` | 32-bit float | General purpose (most common) |
| `DataType::VECTOR_FP64` | 64-bit float | Higher precision |
| `DataType::VECTOR_FP16` | 16-bit float | Memory-efficient storage |
| `DataType::VECTOR_INT8` | 8-bit integer | Quantized embeddings |
| `DataType::VECTOR_INT16` | 16-bit integer | Quantized embeddings |
| `DataType::VECTOR_INT4` | 4-bit integer (packed as int8) | Highly compressed |
| `DataType::VECTOR_BINARY32` | 32-bit unsigned | Binary hash vectors |
| `DataType::VECTOR_BINARY64` | 64-bit unsigned | Binary hash vectors |

```ruby
# 384-dimensional FP32 vector with HNSW cosine index
embedding = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
              dimension:    384,
              index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))
```

## Sparse Vector Types

Sparse vectors are represented as Ruby hashes mapping integer indices to float values.

| Constant | Value Type |
|----------|-----------|
| `DataType::SPARSE_VECTOR_FP32` | 32-bit float |
| `DataType::SPARSE_VECTOR_FP16` | 16-bit float |

```ruby
# Set a sparse vector on a Doc
doc.set_field("sparse_emb", Zvec::DataType::SPARSE_VECTOR_FP32,
              { 42 => 0.8, 99 => 0.3, 1024 => 0.1 })
```

## Array Types

Array fields store variable-length lists of scalar values.

| Constant | Element Type |
|----------|-------------|
| `DataType::ARRAY_STRING` | String |
| `DataType::ARRAY_BINARY` | String |
| `DataType::ARRAY_BOOL` | Boolean |
| `DataType::ARRAY_INT32` | Integer |
| `DataType::ARRAY_INT64` | Integer |
| `DataType::ARRAY_UINT32` | Integer |
| `DataType::ARRAY_UINT64` | Integer |
| `DataType::ARRAY_FLOAT` | Float |
| `DataType::ARRAY_DOUBLE` | Float |

```ruby
tags_field = Zvec::FieldSchema.create("tags", Zvec::DataType::ARRAY_STRING)
```

## Nullable Fields

By default, all fields are non-nullable. Set `nullable: true` to allow `nil` values:

```ruby
bio = Zvec::FieldSchema.create("bio", Zvec::DataType::STRING, nullable: true)
```

Setting a nullable field to `nil`:

```ruby
doc.set_field("bio", Zvec::DataType::STRING, nil)
```

## Collection Schema

A `CollectionSchema` groups fields together. The first STRING field is typically used as the primary key.

```ruby
schema = Zvec::CollectionSchema.create("my_collection", [pk, title, year, embedding])
```

### Schema Inspection

```ruby
schema.name                    # => "my_collection"
schema.all_field_names         # => ["pk", "title", "year", "embedding"]
schema.has_field?("title")     # => true
schema.fields                  # => [FieldSchema, FieldSchema, ...]
schema.vector_fields           # => [FieldSchema] (only vector fields)
schema.forward_fields          # => [FieldSchema] (non-vector fields)
schema.get_field("embedding")  # => FieldSchema or nil
```

## Field Inspection

```ruby
field = schema.get_field("embedding")
field.name           # => "embedding"
field.data_type      # => VECTOR_FP32
field.dimension      # => 384
field.nullable?      # => false
field.vector_field?  # => true
field.dense_vector?  # => true
field.sparse_vector? # => false
field.array_type?    # => false
field.index_type     # => HNSW
field.index_params   # => HnswIndexParams
```
