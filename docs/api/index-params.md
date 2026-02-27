# Index Parameters

Index parameters control how vector and scalar indexes are built. All parameter classes inherit from `Zvec::IndexParams`.

## Inheritance Hierarchy

```
IndexParams (abstract)
  ├── VectorIndexParams (abstract)
  │     ├── HnswIndexParams
  │     ├── FlatIndexParams
  │     └── IVFIndexParams
  └── InvertIndexParams
```

## IndexParams (Base)

All index params share these methods:

| Method | Returns | Description |
|--------|---------|-------------|
| `type` | `IndexType` | HNSW, IVF, FLAT, or INVERT |
| `vector_index_type?` | Boolean | True for vector index types |
| `to_s` | String | Human-readable description |

## VectorIndexParams (Base)

Shared by all vector index types:

| Method | Returns | Description |
|--------|---------|-------------|
| `metric_type` | `MetricType` | Distance metric (COSINE, L2, IP) |
| `metric_type=` | — | Set distance metric |
| `quantize_type` | `QuantizeType` | Quantization (FP16, INT8, INT4, or UNDEFINED) |
| `quantize_type=` | — | Set quantization |

## HnswIndexParams

```ruby
params = Zvec::HnswIndexParams.new(metric_type, m: 50, ef_construction: 500,
           quantize_type: Zvec::QuantizeType::UNDEFINED)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `metric_type` | `MetricType` | required | Distance metric |
| `m` | Integer | `50` | Max connections per node |
| `ef_construction` | Integer | `500` | Search width during index build |
| `quantize_type` | `QuantizeType` | `UNDEFINED` | Optional quantization |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `m` | Integer | Max connections |
| `m=` | — | Set max connections |
| `ef_construction` | Integer | Construction search width |
| `ef_construction=` | — | Set construction search width |

## FlatIndexParams

```ruby
params = Zvec::FlatIndexParams.new(metric_type, quantize_type: Zvec::QuantizeType::UNDEFINED)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `metric_type` | `MetricType` | required | Distance metric |
| `quantize_type` | `QuantizeType` | `UNDEFINED` | Optional quantization |

No additional instance methods beyond `VectorIndexParams`.

## IVFIndexParams

```ruby
params = Zvec::IVFIndexParams.new(metric_type, n_list: 1024, n_iters: 10,
           use_soar: false, quantize_type: Zvec::QuantizeType::UNDEFINED)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `metric_type` | `MetricType` | required | Distance metric |
| `n_list` | Integer | `1024` | Number of clusters |
| `n_iters` | Integer | `10` | K-means iterations |
| `use_soar` | Boolean | `false` | Enable SOAR optimization |
| `quantize_type` | `QuantizeType` | `UNDEFINED` | Optional quantization |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `n_list` | Integer | Number of clusters |
| `n_list=` | — | Set cluster count |
| `n_iters` | Integer | K-means iterations |
| `n_iters=` | — | Set iteration count |
| `use_soar?` | Boolean | SOAR optimization enabled |
| `use_soar=` | — | Set SOAR optimization |

## InvertIndexParams

For scalar field indexes used in filtered queries:

```ruby
params = Zvec::InvertIndexParams.new(
           enable_range_optimization: true,
           enable_extended_wildcard: false)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_range_optimization` | Boolean | `true` | Optimize range queries |
| `enable_extended_wildcard` | Boolean | `false` | Extended wildcard support |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `enable_range_optimization?` | Boolean | Range optimization enabled |
| `enable_range_optimization=` | — | Set range optimization |
| `enable_extended_wildcard?` | Boolean | Extended wildcards enabled |
| `enable_extended_wildcard=` | — | Set extended wildcards |
