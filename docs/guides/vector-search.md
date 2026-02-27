# Vector Search

Vector search finds the most similar documents to a query vector using approximate nearest neighbor (ANN) algorithms.

## Convenience Method: query_vector

The simplest way to search:

```ruby
results = col.query_vector("embedding", query_vec, top_k: 5)
```

Full signature:

```ruby
col.query_vector(
  field_name,          # name of the vector field to search
  vector,              # query vector (Array of floats)
  top_k:,              # number of results to return
  filter: nil,         # optional scalar filter expression
  include_vector: false,  # include vector data in results
  query_params: nil,   # optional QueryParams for tuning
  output_fields: nil   # optional array of field names to return
)
```

## Distance Metrics

The distance metric is set when you define the vector field's index:

| Metric | Constant | Score Meaning |
|--------|----------|---------------|
| Cosine | `MetricType::COSINE` | 0 = identical, 2 = opposite |
| L2 (Euclidean) | `MetricType::L2` | 0 = identical, increases with distance |
| Inner Product | `MetricType::IP` | Higher = more similar (negated internally) |

!!! tip "Converting Cosine Distance to Similarity"
    Zvec returns cosine **distance** (lower is better). To get a similarity score where higher is better:
    ```ruby
    similarity = 1.0 - doc.score
    ```

## Reading Results

Query results are returned as an array of `Doc` objects, sorted by distance (closest first):

```ruby
results = col.query_vector("embedding", query_vec, top_k: 3)

results.each do |doc|
  h = doc.to_h(col.schema)
  puts "#{h['pk']}: #{h['title']} (distance: #{h['score'].round(4)})"
end
```

## Advanced: VectorQuery

For full control, build a `VectorQuery` directly:

```ruby
vq = Zvec::VectorQuery.new
vq.topk = 10
vq.field_name = "embedding"
vq.filter = "year > 2020"
vq.include_vector = true
vq.query_params = Zvec::HnswQueryParams.new(ef: 500)
vq.output_fields = ["title", "year"]

# Set the query vector using the field schema for type dispatch
field_schema = col.schema.get_field("embedding")
vq.set_vector(field_schema, query_vec)

results = col.query(vq)
```

### VectorQuery Properties

| Property | Type | Description |
|----------|------|-------------|
| `topk` | Integer | Number of results to return |
| `field_name` | String | Vector field to search |
| `filter` | String | Scalar filter expression |
| `include_vector` | Boolean | Include vector data in results |
| `include_doc_id` | Boolean | Include internal doc IDs |
| `query_params` | QueryParams | Index-specific search parameters |
| `output_fields` | Array | Specific fields to include in results |

## Group-By Query

Group results by a scalar field value:

```ruby
gq = Zvec::GroupByVectorQuery.new
gq.field_name = "embedding"
gq.group_by_field_name = "category"
gq.group_count = 5       # number of groups
gq.group_topk = 3        # results per group

field_schema = col.schema.get_field("embedding")
gq.set_vector(field_schema, query_vec)

groups = col.group_by_query(gq)
groups.each do |group|
  puts "Group: #{group.group_by_value}"
  group.docs.each do |doc|
    h = doc.to_h(col.schema)
    puts "  #{h['title']}"
  end
end
```

## Output Fields

By default, all scalar fields are returned. Use `output_fields` to limit the response:

```ruby
results = col.query_vector("embedding", query_vec,
            top_k: 5,
            output_fields: ["title", "year"])
```

## Including Vectors

Vector data is excluded from results by default for performance. Set `include_vector: true` to get vectors back:

```ruby
results = col.query_vector("embedding", query_vec,
            top_k: 5,
            include_vector: true)

results.each do |doc|
  h = doc.to_h(col.schema)
  puts h['embedding'].first(5).inspect  # first 5 dimensions
end
```
