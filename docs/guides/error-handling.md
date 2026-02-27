# Error Handling

Zvec maps C++ status codes to a Ruby exception hierarchy. All exceptions inherit from `Zvec::Error`, which inherits from `StandardError`.

## Exception Hierarchy

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

## Exception Details

| Exception | Status Code | Common Cause |
|-----------|-------------|-------------|
| `NotFoundError` | `NOT_FOUND` | Collection path doesn't exist, field not in schema |
| `AlreadyExistsError` | `ALREADY_EXISTS` | Creating a collection at a path that already exists |
| `InvalidArgumentError` | `INVALID_ARGUMENT` | Wrong data type, dimension mismatch, malformed filter |
| `PermissionDeniedError` | `PERMISSION_DENIED` | Writing to a read-only collection |
| `FailedPreconditionError` | `FAILED_PRECONDITION` | Operation requires a condition that isn't met |
| `ResourceExhaustedError` | `RESOURCE_EXHAUSTED` | Memory limit reached |
| `UnavailableError` | `UNAVAILABLE` | Internal resource temporarily unavailable |
| `InternalError` | `INTERNAL_ERROR` | Unexpected internal failure |
| `NotSupportedError` | `NOT_SUPPORTED` | Feature not supported for this configuration |

## Handling Errors

### Catch a Specific Exception

```ruby
begin
  col = Zvec::Collection.open("/nonexistent/path")
rescue Zvec::NotFoundError => e
  puts "Collection not found: #{e.message}"
end
```

### Catch All Zvec Errors

```ruby
begin
  col.insert([doc])
rescue Zvec::Error => e
  puts "Zvec error: #{e.message}"
end
```

### Check Insert Status

Insert, upsert, update, and delete return an array of `Status` objects — one per document. These do not raise exceptions; check them individually:

```ruby
statuses = col.insert([doc1, doc2, doc3])

statuses.each_with_index do |status, i|
  if status.ok?
    puts "Document #{i} inserted"
  else
    puts "Document #{i} failed: #{status.message} (code: #{status.code})"
  end
end
```

## Status Objects

The `Zvec::Status` class wraps C++ status codes:

```ruby
status.ok?      # => true if successful
status.code     # => StatusCode enum value
status.message  # => human-readable error message
status.to_s     # => "OK" or "Error(code): message"
```

## Common Error Scenarios

### Dimension Mismatch

Inserting a vector with the wrong number of dimensions:

```ruby
# Schema defines dimension: 384
doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, [0.1, 0.2])  # only 2 dims
col.insert([doc])  # status will indicate an error
```

### Duplicate Primary Key

Inserting a document with a primary key that already exists:

```ruby
col.insert([doc])  # first insert: OK
col.insert([doc])  # second insert: error status (use upsert instead)
```

### Read-Only Collection

Writing to a collection opened in read-only mode:

```ruby
opts = Zvec::CollectionOptions.new
opts.read_only = true
col = Zvec::Collection.open(path, options: opts)

col.insert([doc])  # raises Zvec::PermissionDeniedError
```
