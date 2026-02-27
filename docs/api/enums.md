# Enums

All enum constants are defined under the `Zvec` module.

## DataType

Field data types, accessed as `Zvec::DataType::CONSTANT`.

### Scalar Types

| Constant | Description |
|----------|-------------|
| `UNDEFINED` | Undefined type |
| `BINARY` | Raw binary data |
| `STRING` | UTF-8 string |
| `BOOL` | Boolean |
| `INT32` | 32-bit signed integer |
| `INT64` | 64-bit signed integer |
| `UINT32` | 32-bit unsigned integer |
| `UINT64` | 64-bit unsigned integer |
| `FLOAT` | 32-bit floating point |
| `DOUBLE` | 64-bit floating point |

### Dense Vector Types

| Constant | Element Type |
|----------|-------------|
| `VECTOR_FP16` | 16-bit float |
| `VECTOR_FP32` | 32-bit float |
| `VECTOR_FP64` | 64-bit float |
| `VECTOR_INT4` | 4-bit integer (packed as int8) |
| `VECTOR_INT8` | 8-bit integer |
| `VECTOR_INT16` | 16-bit integer |
| `VECTOR_BINARY32` | 32-bit unsigned binary |
| `VECTOR_BINARY64` | 64-bit unsigned binary |

### Sparse Vector Types

| Constant | Value Type |
|----------|-----------|
| `SPARSE_VECTOR_FP16` | 16-bit float |
| `SPARSE_VECTOR_FP32` | 32-bit float |

### Array Types

| Constant | Element Type |
|----------|-------------|
| `ARRAY_BINARY` | Binary/String |
| `ARRAY_STRING` | String |
| `ARRAY_BOOL` | Boolean |
| `ARRAY_INT32` | 32-bit integer |
| `ARRAY_INT64` | 64-bit integer |
| `ARRAY_UINT32` | 32-bit unsigned integer |
| `ARRAY_UINT64` | 64-bit unsigned integer |
| `ARRAY_FLOAT` | 32-bit float |
| `ARRAY_DOUBLE` | 64-bit float |

## IndexType

Index algorithms, accessed as `Zvec::IndexType::CONSTANT`.

| Constant | Description |
|----------|-------------|
| `UNDEFINED` | No index |
| `HNSW` | Hierarchical Navigable Small World |
| `IVF` | Inverted File Index |
| `FLAT` | Flat (brute-force) |
| `INVERT` | Inverted index for scalar fields |

## MetricType

Distance metrics, accessed as `Zvec::MetricType::CONSTANT`.

| Constant | Description |
|----------|-------------|
| `UNDEFINED` | Undefined |
| `L2` | Euclidean (L2) distance |
| `IP` | Inner product |
| `COSINE` | Cosine distance |
| `MIPSL2` | Mixed inner product / L2 |

## QuantizeType

Vector quantization levels, accessed as `Zvec::QuantizeType::CONSTANT`.

| Constant | Compression | Description |
|----------|------------|-------------|
| `UNDEFINED` | None | No quantization |
| `FP16` | 2x | Float16 quantization |
| `INT8` | 4x | 8-bit integer quantization |
| `INT4` | 8x | 4-bit integer quantization |

## StatusCode

Operation result codes, accessed as `Zvec::StatusCode::CONSTANT`.

| Constant | Description |
|----------|-------------|
| `OK` | Success |
| `NOT_FOUND` | Resource not found |
| `ALREADY_EXISTS` | Resource already exists |
| `INVALID_ARGUMENT` | Invalid input |
| `PERMISSION_DENIED` | Insufficient permissions |
| `FAILED_PRECONDITION` | Precondition not met |
| `RESOURCE_EXHAUSTED` | Resources exhausted |
| `UNAVAILABLE` | Temporarily unavailable |
| `INTERNAL_ERROR` | Internal error |
| `NOT_SUPPORTED` | Not supported |
| `UNKNOWN` | Unknown error |

## Operator

Document operation types, accessed as `Zvec::Operator::CONSTANT`.

| Constant | Description |
|----------|-------------|
| `INSERT` | Insert new document |
| `UPSERT` | Insert or replace |
| `UPDATE` | Update existing |
| `DELETE` | Delete document |

## LogLevel

Logging severity, accessed as `Zvec::LogLevel::CONSTANT`.

| Constant | Description |
|----------|-------------|
| `DEBUG` | Verbose diagnostics |
| `INFO` | Operational messages |
| `WARN` | Potential issues |
| `ERROR` | Error conditions |
| `FATAL` | Critical failures |
