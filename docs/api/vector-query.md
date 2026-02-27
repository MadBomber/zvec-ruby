# VectorQuery

`Zvec::VectorQuery` provides full control over vector similarity searches. For simple queries, prefer the `query_vector` convenience method on `Collection`.

## Constructor

```ruby
vq = Zvec::VectorQuery.new
```

## Properties

| Method | Type | Description |
|--------|------|-------------|
| `topk` / `topk=` | Integer | Number of results to return |
| `field_name` / `field_name=` | String | Vector field to search |
| `filter` / `filter=` | String | Scalar filter expression |
| `include_vector?` / `include_vector=` | Boolean | Include vector data in results |
| `include_doc_id?` / `include_doc_id=` | Boolean | Include internal doc IDs |
| `query_params` / `query_params=` | `QueryParams` | Index-specific search parameters |
| `output_fields=` | Array | Field names to include in results |

## Setting the Query Vector

### `set_vector(field_schema, data)`

Set the query vector using a `FieldSchema` for type dispatch:

```ruby
field_schema = col.schema.get_field("embedding")

# Dense vector — pass a Ruby Array
vq.set_vector(field_schema, [0.1, 0.2, 0.3, 0.4])

# Sparse vector — pass a Ruby Hash
vq.set_vector(field_schema, { 42 => 0.8, 99 => 0.3 })
```

The method automatically handles dense vs. sparse serialization based on the field schema's data type.

## Complete Example

```ruby
vq = Zvec::VectorQuery.new
vq.topk = 10
vq.field_name = "embedding"
vq.filter = "year > 2020 AND category = 'science'"
vq.include_vector = false
vq.query_params = Zvec::HnswQueryParams.new(ef: 500)
vq.output_fields = ["title", "year", "category"]

field_schema = col.schema.get_field("embedding")
vq.set_vector(field_schema, query_vec)

results = col.query(vq)
results.each do |doc|
  h = doc.to_h(col.schema)
  puts "#{h['title']} — distance: #{h['score'].round(4)}"
end
```

## GroupByVectorQuery

Group results by a scalar field value:

```ruby
gq = Zvec::GroupByVectorQuery.new
```

### Properties

| Method | Type | Description |
|--------|------|-------------|
| `field_name` / `field_name=` | String | Vector field to search |
| `filter` / `filter=` | String | Scalar filter expression |
| `include_vector?` / `include_vector=` | Boolean | Include vector data |
| `group_by_field_name` / `group_by_field_name=` | String | Field to group by |
| `group_count` / `group_count=` | Integer | Number of groups |
| `group_topk` / `group_topk=` | Integer | Results per group |
| `query_params` / `query_params=` | `QueryParams` | Search parameters |
| `set_vector(field_schema, data)` | — | Set the query vector |

### GroupResult

Each group in the results contains:

| Method | Returns | Description |
|--------|---------|-------------|
| `group_by_value` | String | The group key value |
| `docs` | Array | Array of `Doc` objects in this group |
