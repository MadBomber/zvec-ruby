# Status and Errors

## Status

`Zvec::Status` wraps C++ status codes returned by batch operations (insert, upsert, update, delete).

### Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `ok?` | Boolean | True if the operation succeeded |
| `code` | `StatusCode` | Status code enum value |
| `message` | String | Human-readable error message |
| `to_s` | String | `"OK"` or `"Error(code): message"` |

### Usage

```ruby
statuses = col.insert([doc1, doc2])
statuses.each do |s|
  if s.ok?
    puts "Success"
  else
    puts "Failed: #{s.message}"
  end
end
```

## Exception Hierarchy

Operations that fail fatally (like opening a nonexistent collection) raise exceptions instead of returning status objects. All zvec exceptions inherit from `Zvec::Error`:

```
StandardError
  └── Zvec::Error
        ├── Zvec::NotFoundError
        ├── Zvec::AlreadyExistsError
        ├── Zvec::InvalidArgumentError
        ├── Zvec::PermissionDeniedError
        ├── Zvec::FailedPreconditionError
        ├── Zvec::ResourceExhaustedError
        ├── Zvec::UnavailableError
        ├── Zvec::InternalError
        └── Zvec::NotSupportedError
```

## Exception Reference

| Exception | StatusCode | Typical Cause |
|-----------|-----------|---------------|
| `Zvec::Error` | (any) | Base class for all zvec errors |
| `NotFoundError` | `NOT_FOUND` | Collection path doesn't exist |
| `AlreadyExistsError` | `ALREADY_EXISTS` | Collection already exists at path |
| `InvalidArgumentError` | `INVALID_ARGUMENT` | Wrong data type, bad filter, dimension mismatch |
| `PermissionDeniedError` | `PERMISSION_DENIED` | Writing to read-only collection |
| `FailedPreconditionError` | `FAILED_PRECONDITION` | Required precondition not met |
| `ResourceExhaustedError` | `RESOURCE_EXHAUSTED` | Memory limit exceeded |
| `UnavailableError` | `UNAVAILABLE` | Resource temporarily unavailable |
| `InternalError` | `INTERNAL_ERROR` | Unexpected engine failure |
| `NotSupportedError` | `NOT_SUPPORTED` | Feature not available |

## StatusCode Enum

Available as `Zvec::StatusCode`:

| Constant | Description |
|----------|-------------|
| `OK` | Success |
| `NOT_FOUND` | Resource not found |
| `ALREADY_EXISTS` | Resource already exists |
| `INVALID_ARGUMENT` | Invalid input |
| `PERMISSION_DENIED` | Insufficient permissions |
| `FAILED_PRECONDITION` | Precondition failed |
| `RESOURCE_EXHAUSTED` | Resources exhausted |
| `UNAVAILABLE` | Temporarily unavailable |
| `INTERNAL_ERROR` | Internal error |
| `NOT_SUPPORTED` | Not supported |
| `UNKNOWN` | Unknown error |
