#include "zvec_common.hpp"

using namespace Rice;

// Helper to check if a Ruby Hash contains a given symbol key
static bool hash_has(Rice::Hash& h, const char* key) {
  VALUE sym = rb_id2sym(rb_intern(key));
  VALUE val = rb_hash_aref(h.value(), sym);
  return !NIL_P(val);
}

// Helper to get a VALUE from a Ruby Hash by symbol key
static VALUE hash_get(Rice::Hash& h, const char* key) {
  VALUE sym = rb_id2sym(rb_intern(key));
  return rb_hash_aref(h.value(), sym);
}

void init_zvec_config(Rice::Module& m) {
  // LogLevel enum
  Rice::define_enum_under<zvec::GlobalConfig::LogLevel>("LogLevel", m)
    .define_value("DEBUG", zvec::GlobalConfig::LogLevel::DEBUG)
    .define_value("INFO", zvec::GlobalConfig::LogLevel::INFO)
    .define_value("WARN", zvec::GlobalConfig::LogLevel::WARN)
    .define_value("ERROR", zvec::GlobalConfig::LogLevel::ERROR)
    .define_value("FATAL", zvec::GlobalConfig::LogLevel::FATAL);

  // Zvec.configure(options_hash) module function
  m.define_module_function("configure", [](Rice::Hash opts) {
    zvec::GlobalConfig::ConfigData config;

    // memory_limit_mb — convert MB to bytes
    if (hash_has(opts, "memory_limit_mb")) {
      auto mb = Rice::detail::From_Ruby<uint64_t>().convert(hash_get(opts, "memory_limit_mb"));
      config.memory_limit_bytes = mb * 1024 * 1024;
    }

    // query_thread_count
    if (hash_has(opts, "query_thread_count")) {
      config.query_thread_count = Rice::detail::From_Ruby<uint32_t>().convert(
        hash_get(opts, "query_thread_count"));
    }

    // optimize_thread_count
    if (hash_has(opts, "optimize_thread_count")) {
      config.optimize_thread_count = Rice::detail::From_Ruby<uint32_t>().convert(
        hash_get(opts, "optimize_thread_count"));
    }

    // invert_to_forward_scan_ratio
    if (hash_has(opts, "invert_to_forward_scan_ratio")) {
      config.invert_to_forward_scan_ratio = Rice::detail::From_Ruby<float>().convert(
        hash_get(opts, "invert_to_forward_scan_ratio"));
    }

    // brute_force_by_keys_ratio
    if (hash_has(opts, "brute_force_by_keys_ratio")) {
      config.brute_force_by_keys_ratio = Rice::detail::From_Ruby<float>().convert(
        hash_get(opts, "brute_force_by_keys_ratio"));
    }

    // Determine log level (used by both log types)
    auto level = zvec::GlobalConfig::LogLevel::WARN;
    if (hash_has(opts, "log_level")) {
      level = Rice::detail::From_Ruby<zvec::GlobalConfig::LogLevel>().convert(
        hash_get(opts, "log_level"));
    }

    // log_type: "console" (default) or "file"
    if (hash_has(opts, "log_type")) {
      auto log_type = Rice::detail::From_Ruby<std::string>().convert(
        hash_get(opts, "log_type"));

      if (log_type == "file") {
        std::string dir = "./logs";
        std::string basename = "zvec.log";
        uint32_t file_size = 2048;
        uint32_t overdue_days = 7;

        if (hash_has(opts, "log_dir"))
          dir = Rice::detail::From_Ruby<std::string>().convert(hash_get(opts, "log_dir"));
        if (hash_has(opts, "log_basename"))
          basename = Rice::detail::From_Ruby<std::string>().convert(hash_get(opts, "log_basename"));
        if (hash_has(opts, "log_file_size"))
          file_size = Rice::detail::From_Ruby<uint32_t>().convert(hash_get(opts, "log_file_size"));
        if (hash_has(opts, "log_overdue_days"))
          overdue_days = Rice::detail::From_Ruby<uint32_t>().convert(hash_get(opts, "log_overdue_days"));

        config.log_config = std::make_shared<zvec::GlobalConfig::FileLogConfig>(
          level, dir, basename, file_size, overdue_days);
      } else {
        config.log_config = std::make_shared<zvec::GlobalConfig::ConsoleLogConfig>(level);
      }
    } else if (hash_has(opts, "log_level")) {
      config.log_config = std::make_shared<zvec::GlobalConfig::ConsoleLogConfig>(level);
    }

    auto& gc = zvec::GlobalConfig::Instance();
    zvec_rb::throw_if_error(gc.Initialize(config));
  });
}
