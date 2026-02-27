# CollectionSchema

`Zvec::CollectionSchema` groups multiple `FieldSchema` definitions into a complete schema for a collection.

## Class Methods

### `CollectionSchema.create`

```ruby
Zvec::CollectionSchema.create(name, fields)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `name` | String | Schema name (typically matches the collection name) |
| `fields` | Array | Array of `FieldSchema` instances |

**Example:**

```ruby
schema = Zvec::CollectionSchema.create("movies", [pk, title, year, embedding])
```

### `CollectionSchema.new`

```ruby
Zvec::CollectionSchema.new(name)
```

Creates an empty schema. Use `add_field` to add fields individually.

## Instance Methods

### Properties

| Method | Returns | Description |
|--------|---------|-------------|
| `name` | String | Schema name |

### Field Access

| Method | Returns | Description |
|--------|---------|-------------|
| `fields` | Array | All field schemas |
| `vector_fields` | Array | Only vector field schemas |
| `forward_fields` | Array | Non-vector (scalar) field schemas |
| `all_field_names` | Array | Field names as strings |
| `get_field(name)` | `FieldSchema` or `nil` | Look up a field by name |
| `has_field?(name)` | Boolean | Check if a field exists |

### Mutation

| Method | Parameters | Description |
|--------|-----------|-------------|
| `add_field(field_schema)` | `FieldSchema` | Add a field to the schema |

### Display

| Method | Returns | Description |
|--------|---------|-------------|
| `to_s` | String | Human-readable schema description |

## CollectionStats

Returned by `Collection#stats`:

| Method | Returns | Description |
|--------|---------|-------------|
| `doc_count` | Integer | Number of documents in the collection |
| `index_completeness` | Hash | `{ field_name => completeness_ratio }` mapping |
| `to_s` | String | Human-readable stats |
