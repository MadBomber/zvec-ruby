# Global Configuration

`Zvec.configure` sets engine-wide settings for memory, threading, and logging. Call it before creating or opening any collections.

## Usage

```ruby
Zvec.configure(options_hash)
```

## Options

| Key | Type | Description |
|-----|------|-------------|
| `memory_limit_mb` | Integer | Maximum memory usage in megabytes |
| `query_thread_count` | Integer | Threads for query execution |
| `optimize_thread_count` | Integer | Threads for index optimization |
| `invert_to_forward_scan_ratio` | Float | Threshold for inverted-index vs. forward-scan |
| `brute_force_by_keys_ratio` | Float | Threshold for brute-force search by keys |

## Logging Options

| Key | Type | Description |
|-----|------|-------------|
| `log_type` | String | `"console"` (default) or `"file"` |
| `log_level` | `LogLevel` | Minimum severity to log |

### File Logging Options

These apply when `log_type: "file"`:

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_dir` | String | `"./logs"` | Log directory path |
| `log_basename` | String | `"zvec.log"` | Log file name |
| `log_file_size` | Integer | `2048` | Max file size in KB |
| `log_overdue_days` | Integer | `7` | Days before rotation |

## LogLevel Enum

Available as `Zvec::LogLevel`:

| Constant | Description |
|----------|-------------|
| `DEBUG` | Verbose diagnostics |
| `INFO` | General operational messages |
| `WARN` | Potential issues (default) |
| `ERROR` | Error conditions |
| `FATAL` | Critical failures |

## Examples

### Minimal

```ruby
Zvec.configure(memory_limit_mb: 256)
```

### Full Configuration

```ruby
Zvec.configure(
  memory_limit_mb: 512,
  query_thread_count: 4,
  optimize_thread_count: 2,
  log_type: "file",
  log_level: Zvec::LogLevel::INFO,
  log_dir: "/var/log/zvec",
  log_basename: "search.log",
  log_file_size: 4096,
  log_overdue_days: 14
)
```
