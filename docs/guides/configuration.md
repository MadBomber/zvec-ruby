# Configuration

Zvec provides global configuration for memory limits, threading, and logging. Call `Zvec.configure` before creating or opening collections.

## Basic Configuration

```ruby
Zvec.configure(
  memory_limit_mb: 512,
  query_thread_count: 4,
  optimize_thread_count: 2
)
```

## Options

| Option | Type | Description |
|--------|------|-------------|
| `memory_limit_mb` | Integer | Maximum memory usage in megabytes |
| `query_thread_count` | Integer | Number of threads for query execution |
| `optimize_thread_count` | Integer | Number of threads for index optimization |
| `invert_to_forward_scan_ratio` | Float | Threshold for switching between inverted index lookup and forward scan |
| `brute_force_by_keys_ratio` | Float | Threshold for switching to brute-force search by keys |

## Logging

### Console Logging

```ruby
Zvec.configure(
  log_type: "console",
  log_level: Zvec::LogLevel::INFO
)
```

### File Logging

```ruby
Zvec.configure(
  log_type: "file",
  log_level: Zvec::LogLevel::WARN,
  log_dir: "./logs",
  log_basename: "zvec.log",
  log_file_size: 2048,       # max file size in KB
  log_overdue_days: 7         # days before log rotation
)
```

### Log Levels

| Level | Constant | Description |
|-------|----------|-------------|
| Debug | `LogLevel::DEBUG` | Verbose diagnostic output |
| Info | `LogLevel::INFO` | General operational messages |
| Warn | `LogLevel::WARN` | Potential issues (default) |
| Error | `LogLevel::ERROR` | Error conditions |
| Fatal | `LogLevel::FATAL` | Critical failures |

## Configuration Timing

`Zvec.configure` must be called before any collections are created or opened. Calling it after initialization raises an error.

```ruby
# Correct order
Zvec.configure(memory_limit_mb: 256)
col = Zvec::Collection.create_and_open(path, schema)
```
