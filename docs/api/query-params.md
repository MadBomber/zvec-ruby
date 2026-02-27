# Query Parameters

Query parameters control search-time behavior for vector queries. Each index type has its own query parameter class.

## Inheritance Hierarchy

```
QueryParams (abstract)
  ├── HnswQueryParams
  ├── IVFQueryParams
  └── FlatQueryParams
```

## QueryParams (Base)

Shared by all query parameter types:

| Method | Returns | Description |
|--------|---------|-------------|
| `type` | `IndexType` | Index type this applies to |
| `radius` | Float | Max distance threshold |
| `radius=` | — | Set max distance threshold |
| `linear?` | Boolean | Force linear (brute-force) scan |
| `linear=` | — | Set linear scan mode |
| `using_refiner?` | Boolean | Re-rank with exact distances |
| `using_refiner=` | — | Set refiner mode |

## HnswQueryParams

```ruby
qp = Zvec::HnswQueryParams.new(ef: 300, radius: 0.0, is_linear: false, is_using_refiner: false)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `ef` | Integer | `300` | Search width at query time |
| `radius` | Float | `0.0` | Max distance (0 = unlimited) |
| `is_linear` | Boolean | `false` | Bypass HNSW, use brute force |
| `is_using_refiner` | Boolean | `false` | Re-rank candidates exactly |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `ef` | Integer | Search width |
| `ef=` | — | Set search width |

## IVFQueryParams

```ruby
qp = Zvec::IVFQueryParams.new(nprobe: 10, is_using_refiner: false, scale_factor: 10.0)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `nprobe` | Integer | `10` | Number of clusters to search |
| `is_using_refiner` | Boolean | `false` | Re-rank candidates exactly |
| `scale_factor` | Float | `10.0` | Internal scaling parameter |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `nprobe` | Integer | Clusters to search |
| `nprobe=` | — | Set probe count |
| `scale_factor` | Float | Scale factor |
| `scale_factor=` | — | Set scale factor |

## FlatQueryParams

```ruby
qp = Zvec::FlatQueryParams.new(is_using_refiner: false, scale_factor: 10.0)
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `is_using_refiner` | Boolean | `false` | Re-rank candidates |
| `scale_factor` | Float | `10.0` | Internal scaling parameter |

**Instance methods:**

| Method | Returns | Description |
|--------|---------|-------------|
| `scale_factor` | Float | Scale factor |
| `scale_factor=` | — | Set scale factor |

## Usage with query_vector

Pass query params as the `query_params` keyword argument:

```ruby
qp = Zvec::HnswQueryParams.new(ef: 500)
results = col.query_vector("embedding", query_vec, top_k: 10, query_params: qp)
```
