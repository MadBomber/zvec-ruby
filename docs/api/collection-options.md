# CollectionOptions

`Zvec::CollectionOptions` controls how a collection is opened.

## Constructor

```ruby
opts = Zvec::CollectionOptions.new
```

Creates options with default values.

## Properties

| Method | Type | Default | Description |
|--------|------|---------|-------------|
| `read_only?` / `read_only=` | Boolean | `false` | Open in read-only mode |
| `enable_mmap?` / `enable_mmap=` | Boolean | `false` | Memory-map data files |
| `max_buffer_size` / `max_buffer_size=` | Integer | (engine default) | Write buffer size |

## Usage

```ruby
opts = Zvec::CollectionOptions.new
opts.read_only = true
opts.enable_mmap = true
opts.max_buffer_size = 4096

col = Zvec::Collection.open("/path/to/collection", options: opts)
```

Or when creating a new collection:

```ruby
col = Zvec::Collection.create_and_open("/path/to/collection", schema, options: opts)
```

## Read-Only Mode

When `read_only` is true, all write operations (`insert`, `upsert`, `update`, `delete`, `flush`) raise `Zvec::PermissionDeniedError`.

## Memory Mapping

When `enable_mmap` is true, data files are memory-mapped instead of loaded into the heap. This can reduce memory usage for large collections at the cost of potentially slower random access.
