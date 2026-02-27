# FieldSchema

`Zvec::FieldSchema` defines a single field within a collection schema, including its name, data type, vector dimensionality, nullability, and index configuration.

## Class Methods

### `FieldSchema.create`

```ruby
Zvec::FieldSchema.create(name, data_type, dimension: 0, nullable: false, index_params: nil)
```

Creates a new field schema.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | String | required | Field name |
| `data_type` | `DataType` | required | Data type constant |
| `dimension` | Integer | `0` | Vector dimensionality (required for vector fields) |
| `nullable` | Boolean | `false` | Allow nil values |
| `index_params` | `IndexParams` | `nil` | Index configuration |

Returns a `FieldSchema` instance (wrapped in a shared pointer).

**Examples:**

```ruby
# Scalar field
pk = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)

# Nullable scalar
bio = Zvec::FieldSchema.create("bio", Zvec::DataType::STRING, nullable: true)

# Vector field with HNSW index
embedding = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
              dimension: 384,
              index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))
```

### `FieldSchema.new`

```ruby
Zvec::FieldSchema.new(name, data_type)
```

Low-level constructor. Prefer `FieldSchema.create` which accepts keyword arguments.

## Instance Methods

### Properties

| Method | Returns | Description |
|--------|---------|-------------|
| `name` | String | Field name |
| `data_type` | `DataType` | Data type enum |
| `dimension` | Integer | Vector dimensionality (0 for scalars) |
| `nullable?` | Boolean | Whether nil values are allowed |
| `index_type` | `IndexType` | Type of index (HNSW, IVF, FLAT, INVERT, or UNDEFINED) |
| `index_params` | `IndexParams` | Index parameters (nil if no index) |

### Setters

| Method | Parameter | Description |
|--------|-----------|-------------|
| `dimension=` | Integer | Set vector dimensionality |
| `nullable=` | Boolean | Set nullability |
| `index_params=` | `IndexParams` | Set index parameters |

### Type Checks

| Method | Returns | Description |
|--------|---------|-------------|
| `vector_field?` | Boolean | True for any vector type |
| `dense_vector?` | Boolean | True for dense vector types |
| `sparse_vector?` | Boolean | True for sparse vector types |
| `array_type?` | Boolean | True for array types |
| `invert_index?` | Boolean | True if field has an inverted index |

### Display

| Method | Returns | Description |
|--------|---------|-------------|
| `to_s` | String | Human-readable description |
