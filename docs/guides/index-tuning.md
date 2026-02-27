# Index Tuning

Zvec supports three vector index types, each with tunable parameters that trade off between search speed, accuracy, and memory usage.

## HNSW (Hierarchical Navigable Small World)

The default and most versatile index type. Good for most use cases.

### Index Parameters

```ruby
params = Zvec::HnswIndexParams.new(
  Zvec::MetricType::COSINE,  # distance metric
  m: 50,                      # max connections per node (default: 50)
  ef_construction: 500         # search width during build (default: 500)
)
```

| Parameter | Default | Effect |
|-----------|---------|--------|
| `m` | 50 | Higher = better recall, more memory, slower inserts |
| `ef_construction` | 500 | Higher = better index quality, slower build time |

### Query Parameters

```ruby
qp = Zvec::HnswQueryParams.new(
  ef: 300,                # search width at query time (default: 300)
  radius: 0.0,            # max distance threshold (0 = unlimited)
  is_linear: false,        # force linear scan instead of HNSW
  is_using_refiner: false  # re-rank with exact distances
)

results = col.query_vector("embedding", query_vec, top_k: 10, query_params: qp)
```

| Parameter | Default | Effect |
|-----------|---------|--------|
| `ef` | 300 | Higher = better recall, slower query |
| `radius` | 0.0 | Filter results beyond this distance (0 = no filter) |
| `linear` | false | Bypass HNSW index for exact brute-force search |
| `using_refiner` | false | Re-rank candidates with exact distance computation |

!!! tip "Tuning ef"
    Start with `ef` equal to `top_k * 10`. Increase if recall is insufficient. Decrease for faster queries when approximate results are acceptable.

## IVF (Inverted File Index)

Partitions vectors into clusters. Good for larger datasets where HNSW memory usage is a concern.

### Index Parameters

```ruby
params = Zvec::IVFIndexParams.new(
  Zvec::MetricType::COSINE,
  n_list: 1024,       # number of clusters (default: 1024)
  n_iters: 10,        # k-means iterations during build (default: 10)
  use_soar: false      # use SOAR optimization (default: false)
)
```

| Parameter | Default | Effect |
|-----------|---------|--------|
| `n_list` | 1024 | More clusters = finer partitioning, higher build cost |
| `n_iters` | 10 | More iterations = better cluster quality, slower build |
| `use_soar` | false | Enable SOAR optimization for better recall |

### Query Parameters

```ruby
qp = Zvec::IVFQueryParams.new(
  nprobe: 10,              # clusters to search (default: 10)
  is_using_refiner: false,  # re-rank with exact distances
  scale_factor: 10.0        # internal scaling factor (default: 10.0)
)
```

| Parameter | Default | Effect |
|-----------|---------|--------|
| `nprobe` | 10 | More probes = better recall, slower query |
| `scale_factor` | 10.0 | Internal tuning parameter |

## Flat (Exact Search)

Brute-force exact search with no approximation. Best for small datasets or when perfect recall is required.

### Index Parameters

```ruby
params = Zvec::FlatIndexParams.new(Zvec::MetricType::COSINE)
```

No tuning parameters — every query compares against all vectors.

### Query Parameters

```ruby
qp = Zvec::FlatQueryParams.new(
  is_using_refiner: false,
  scale_factor: 10.0
)
```

## Inverted Index (Scalar Fields)

For scalar fields used in filters, an inverted index speeds up predicate evaluation:

```ruby
params = Zvec::InvertIndexParams.new(
  enable_range_optimization: true,   # optimize range queries (default: true)
  enable_extended_wildcard: false     # extended wildcard patterns (default: false)
)
```

## Quantization

All vector index types support quantization to reduce memory usage at the cost of some precision:

```ruby
params = Zvec::HnswIndexParams.new(
  Zvec::MetricType::COSINE,
  quantize_type: Zvec::QuantizeType::INT8  # quantize from FP32 to INT8
)
```

| Quantization | Compression | Precision Loss |
|-------------|------------|----------------|
| `QuantizeType::FP16` | 2x | Minimal |
| `QuantizeType::INT8` | 4x | Low |
| `QuantizeType::INT4` | 8x | Moderate |

## Index Lifecycle

### Adding an Index After Creation

```ruby
params = Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE)
col.create_index("embedding", params, concurrency: 4)
```

### Dropping an Index

```ruby
col.drop_index("embedding")
```

### Optimizing

Compact and rebuild indexes for better performance:

```ruby
col.optimize(concurrency: 4)
```

## Choosing an Index Type

| Factor | HNSW | IVF | Flat |
|--------|------|-----|------|
| Dataset size | Small to large | Medium to large | Small |
| Memory usage | High | Medium | Low |
| Build time | Medium | High | None |
| Query speed | Fast | Fast | Slow (linear) |
| Recall | Very high | High | Perfect |
| Best for | General use | Large, memory-constrained | < 10K vectors, exact results |
