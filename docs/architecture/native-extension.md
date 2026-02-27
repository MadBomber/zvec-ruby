# Native Extension

The native extension bridges zvec's C++ API to Ruby using [Rice](https://github.com/ruby-rice/rice) v4.11.

## File Organization

Binding files are split by zvec domain. They are initialized in dependency order from `zvec_ext.cpp`:

| File | Purpose | Dependencies |
|------|---------|-------------|
| `zvec_types.cpp` | Enum constants (DataType, MetricType, etc.) | None |
| `zvec_status.cpp` | Status class and exception hierarchy | Types |
| `zvec_params.cpp` | Index params, query params, CollectionOptions, VectorQuery | Types |
| `zvec_schema.cpp` | FieldSchema, CollectionSchema, CollectionStats | Params |
| `zvec_doc.cpp` | Doc with typed field get/set | Schema, Types |
| `zvec_collection.cpp` | Collection CRUD and query operations | All above |
| `zvec_config.cpp` | Global configuration | Status |

## Error Handling Pattern

`zvec_common.hpp` defines two error-handling helpers used throughout the bindings:

### `throw_if_error(status)`

Converts a C++ `Status` to the appropriate Ruby exception:

```cpp
void throw_if_error(const zvec::Status& status) {
  if (status.ok()) return;
  // maps StatusCode to Zvec::*Error and calls rb_raise
}
```

### `unwrap_result(expected)`

Unwraps a `tl::expected<T, Status>`, returning the value on success or raising on error:

```cpp
template <typename T>
T unwrap_result(const tl::expected<T, zvec::Status>& result) {
  if (result.has_value()) return result.value();
  throw_if_error(result.error());
}
```

## The SharedPtr Pattern

Rice wraps `shared_ptr<Collection>` as `Std::SharedPtr<zvec::Collection>`, a proxy class that delegates method calls to the underlying C++ object via `method_missing`.

Ruby convenience methods (like `query_vector`) cannot be defined on `Zvec::Collection` directly because Rice's proxy class isn't the same as `Zvec::Collection`. Instead, `lib/zvec.rb` uses `ObjectSpace` to discover the proxy class at load time and mix in the `CollectionConvenience` module:

```ruby
ObjectSpace.each_object(Class) do |klass|
  if klass.name&.start_with?("Std::SharedPtr") && klass.name&.include?("Collection")
    klass.include(Zvec::CollectionConvenience)
  end
end
```

This is the key architectural detail that enables the `query_vector` helper and the `to_h` doc conversion to work naturally.

## Type Dispatch in Doc

`zvec_doc.cpp` implements `doc_set_field` and `doc_get_field` as large `switch` statements over `DataType`. This is necessary because C++ templates can't be dispatched at runtime — each type requires explicit instantiation:

```cpp
case zvec::DataType::STRING:
  doc.set<std::string>(name, ...);
  break;
case zvec::DataType::INT32:
  doc.set<int32_t>(name, ...);
  break;
case zvec::DataType::VECTOR_FP32: {
  // Convert Ruby Array → std::vector<float>
  Rice::Array arr(value);
  std::vector<float> vec(arr.size());
  for (size_t i = 0; i < arr.size(); i++)
    vec[i] = From_Ruby<float>().convert(arr[i].value());
  doc.set<std::vector<float>>(name, std::move(vec));
  break;
}
```

The Ruby-side `Doc#to_h(schema)` method leverages this by iterating over field names and calling `get_field` with the data type from the schema.

## Sparse Vector Serialization

Sparse vectors are represented differently in Ruby (Hash) and C++ (pair of index/value vectors). The binding handles the conversion:

- **Ruby → C++**: Hash `{ 42 => 0.8, 99 => 0.3 }` → `pair<vector<uint32_t>, vector<float>>`
- **C++ → Ruby**: `pair<vector<uint32_t>, vector<float>>` → Hash `{ 42 => 0.8, 99 => 0.3 }`
