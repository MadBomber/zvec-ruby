#include "zvec_common.hpp"

using namespace Rice;

static inline Rice::Object rb_nil() { return Rice::Object(Qnil); }

// Helper to serialize a Ruby Array of floats into a binary string buffer
static std::string serialize_float_array(Rice::Array ruby_arr) {
  size_t n = ruby_arr.size();
  std::vector<float> vec(n);
  for (size_t i = 0; i < n; i++) {
    vec[i] = Rice::detail::From_Ruby<float>().convert(ruby_arr[i].value());
  }
  std::string buf(reinterpret_cast<const char*>(vec.data()), n * sizeof(float));
  return buf;
}

// Helper to serialize sparse vector from Ruby Hash {uint32 => float}
static std::pair<std::string, std::string> serialize_sparse_vector(Rice::Hash ruby_hash) {
  std::vector<uint32_t> indices;
  std::vector<float> values;
  indices.reserve(ruby_hash.size());
  values.reserve(ruby_hash.size());

  for (auto it = ruby_hash.begin(); it != ruby_hash.end(); ++it) {
    indices.push_back(Rice::detail::From_Ruby<uint32_t>().convert((*it).first.value()));
    values.push_back(Rice::detail::From_Ruby<float>().convert((*it).second.value()));
  }

  std::string idx_buf(reinterpret_cast<const char*>(indices.data()),
                      indices.size() * sizeof(uint32_t));
  std::string val_buf(reinterpret_cast<const char*>(values.data()),
                      values.size() * sizeof(float));
  return {idx_buf, val_buf};
}

void init_zvec_params(Rice::Module& m) {
  // --- IndexParams hierarchy ---

  // Base IndexParams (abstract — no constructor exposed to Ruby)
  Rice::define_class_under<zvec::IndexParams>(m, "IndexParams")
    .define_method("type", &zvec::IndexParams::type)
    .define_method("vector_index_type?", &zvec::IndexParams::is_vector_index_type)
    .define_method("to_s", &zvec::IndexParams::to_string);

  // VectorIndexParams (abstract)
  Rice::define_class_under<zvec::VectorIndexParams, zvec::IndexParams>(m, "VectorIndexParams")
    .define_method("metric_type", &zvec::VectorIndexParams::metric_type)
    .define_method("metric_type=", &zvec::VectorIndexParams::set_metric_type)
    .define_method("quantize_type", &zvec::VectorIndexParams::quantize_type)
    .define_method("quantize_type=", &zvec::VectorIndexParams::set_quantize_type);

  // HnswIndexParams
  Rice::define_class_under<zvec::HnswIndexParams, zvec::VectorIndexParams>(m, "HnswIndexParams")
    .define_constructor(Rice::Constructor<zvec::HnswIndexParams, zvec::MetricType, int, int, zvec::QuantizeType>(),
      Rice::Arg("metric_type"),
      Rice::Arg("m") = 50,
      Rice::Arg("ef_construction") = 500,
      Rice::Arg("quantize_type") = zvec::QuantizeType::UNDEFINED)
    .define_method("m", &zvec::HnswIndexParams::m)
    .define_method("m=", &zvec::HnswIndexParams::set_m)
    .define_method("ef_construction", &zvec::HnswIndexParams::ef_construction)
    .define_method("ef_construction=", &zvec::HnswIndexParams::set_ef_construction);

  // FlatIndexParams
  Rice::define_class_under<zvec::FlatIndexParams, zvec::VectorIndexParams>(m, "FlatIndexParams")
    .define_constructor(Rice::Constructor<zvec::FlatIndexParams, zvec::MetricType, zvec::QuantizeType>(),
      Rice::Arg("metric_type"),
      Rice::Arg("quantize_type") = zvec::QuantizeType::UNDEFINED);

  // IVFIndexParams
  Rice::define_class_under<zvec::IVFIndexParams, zvec::VectorIndexParams>(m, "IVFIndexParams")
    .define_constructor(Rice::Constructor<zvec::IVFIndexParams, zvec::MetricType, int, int, bool, zvec::QuantizeType>(),
      Rice::Arg("metric_type"),
      Rice::Arg("n_list") = 1024,
      Rice::Arg("n_iters") = 10,
      Rice::Arg("use_soar") = false,
      Rice::Arg("quantize_type") = zvec::QuantizeType::UNDEFINED)
    .define_method("n_list", &zvec::IVFIndexParams::n_list)
    .define_method("n_list=", &zvec::IVFIndexParams::set_n_list)
    .define_method("n_iters", &zvec::IVFIndexParams::n_iters)
    .define_method("n_iters=", &zvec::IVFIndexParams::set_n_iters)
    .define_method("use_soar?", &zvec::IVFIndexParams::use_soar)
    .define_method("use_soar=", &zvec::IVFIndexParams::set_use_soar);

  // InvertIndexParams (scalar index)
  Rice::define_class_under<zvec::InvertIndexParams, zvec::IndexParams>(m, "InvertIndexParams")
    .define_constructor(Rice::Constructor<zvec::InvertIndexParams, bool, bool>(),
      Rice::Arg("enable_range_optimization") = true,
      Rice::Arg("enable_extended_wildcard") = false)
    .define_method("enable_range_optimization?", &zvec::InvertIndexParams::enable_range_optimization)
    .define_method("enable_range_optimization=", &zvec::InvertIndexParams::set_enable_range_optimization)
    .define_method("enable_extended_wildcard?", &zvec::InvertIndexParams::enable_extended_wildcard)
    .define_method("enable_extended_wildcard=", &zvec::InvertIndexParams::set_enable_extended_wildcard);

  // --- QueryParams hierarchy ---

  // Base QueryParams (abstract)
  Rice::define_class_under<zvec::QueryParams>(m, "QueryParams")
    .define_method("type", &zvec::QueryParams::type)
    .define_method("radius", &zvec::QueryParams::radius)
    .define_method("radius=", &zvec::QueryParams::set_radius)
    .define_method("linear?", &zvec::QueryParams::is_linear)
    .define_method("linear=", &zvec::QueryParams::set_is_linear)
    .define_method("using_refiner?", &zvec::QueryParams::is_using_refiner)
    .define_method("using_refiner=", &zvec::QueryParams::set_is_using_refiner);

  // HnswQueryParams
  Rice::define_class_under<zvec::HnswQueryParams, zvec::QueryParams>(m, "HnswQueryParams")
    .define_constructor(Rice::Constructor<zvec::HnswQueryParams, int, float, bool, bool>(),
      Rice::Arg("ef") = 300,
      Rice::Arg("radius") = 0.0f,
      Rice::Arg("is_linear") = false,
      Rice::Arg("is_using_refiner") = false)
    .define_method("ef", &zvec::HnswQueryParams::ef)
    .define_method("ef=", &zvec::HnswQueryParams::set_ef);

  // IVFQueryParams
  Rice::define_class_under<zvec::IVFQueryParams, zvec::QueryParams>(m, "IVFQueryParams")
    .define_constructor(Rice::Constructor<zvec::IVFQueryParams, int, bool, float>(),
      Rice::Arg("nprobe") = 10,
      Rice::Arg("is_using_refiner") = false,
      Rice::Arg("scale_factor") = 10.0f)
    .define_method("nprobe", &zvec::IVFQueryParams::nprobe)
    .define_method("nprobe=", &zvec::IVFQueryParams::set_nprobe)
    .define_method("scale_factor", &zvec::IVFQueryParams::scale_factor)
    .define_method("scale_factor=", &zvec::IVFQueryParams::set_scale_factor);

  // FlatQueryParams
  Rice::define_class_under<zvec::FlatQueryParams, zvec::QueryParams>(m, "FlatQueryParams")
    .define_constructor(Rice::Constructor<zvec::FlatQueryParams, bool, float>(),
      Rice::Arg("is_using_refiner") = false,
      Rice::Arg("scale_factor") = 10.0f)
    .define_method("scale_factor", &zvec::FlatQueryParams::scale_factor)
    .define_method("scale_factor=", &zvec::FlatQueryParams::set_scale_factor);

  // --- CollectionOptions ---
  Rice::define_class_under<zvec::CollectionOptions>(m, "CollectionOptions")
    .define_constructor(Rice::Constructor<zvec::CollectionOptions>())
    .define_method("read_only?", [](const zvec::CollectionOptions& o) { return o.read_only_; })
    .define_method("read_only=", [](zvec::CollectionOptions& o, bool v) { o.read_only_ = v; })
    .define_method("enable_mmap?", [](const zvec::CollectionOptions& o) { return o.enable_mmap_; })
    .define_method("enable_mmap=", [](zvec::CollectionOptions& o, bool v) { o.enable_mmap_ = v; })
    .define_method("max_buffer_size", [](const zvec::CollectionOptions& o) { return o.max_buffer_size_; })
    .define_method("max_buffer_size=", [](zvec::CollectionOptions& o, uint32_t v) { o.max_buffer_size_ = v; });

  // --- VectorQuery ---
  Rice::define_class_under<zvec::VectorQuery>(m, "VectorQuery")
    .define_constructor(Rice::Constructor<zvec::VectorQuery>())
    .define_method("topk", [](const zvec::VectorQuery& q) { return q.topk_; })
    .define_method("topk=", [](zvec::VectorQuery& q, int v) { q.topk_ = v; })
    .define_method("field_name", [](const zvec::VectorQuery& q) { return q.field_name_; })
    .define_method("field_name=", [](zvec::VectorQuery& q, std::string v) { q.field_name_ = std::move(v); })
    .define_method("filter", [](const zvec::VectorQuery& q) { return q.filter_; })
    .define_method("filter=", [](zvec::VectorQuery& q, std::string v) { q.filter_ = std::move(v); })
    .define_method("include_vector?", [](const zvec::VectorQuery& q) { return q.include_vector_; })
    .define_method("include_vector=", [](zvec::VectorQuery& q, bool v) { q.include_vector_ = v; })
    .define_method("include_doc_id?", [](const zvec::VectorQuery& q) { return q.include_doc_id_; })
    .define_method("include_doc_id=", [](zvec::VectorQuery& q, bool v) { q.include_doc_id_ = v; })
    .define_method("query_params", [](const zvec::VectorQuery& q) { return q.query_params_; })
    .define_method("query_params=", [](zvec::VectorQuery& q, zvec::QueryParams::Ptr p) { q.query_params_ = p; })
    .define_method("output_fields=", [](zvec::VectorQuery& q, Rice::Array fields) {
      std::vector<std::string> vec;
      vec.reserve(fields.size());
      for (size_t i = 0; i < fields.size(); i++) {
        vec.push_back(Rice::detail::From_Ruby<std::string>().convert(fields[i].value()));
      }
      q.output_fields_ = std::move(vec);
    })
    .define_method("set_vector", [](zvec::VectorQuery& q,
                                    const zvec::FieldSchema& fs,
                                    Rice::Object ruby_data) {
      auto dt = fs.data_type();
      if (zvec::FieldSchema::is_sparse_vector_field(dt)) {
        // Sparse vector: Ruby Hash {index => value}
        Rice::Hash h(ruby_data);
        auto [idx_buf, val_buf] = serialize_sparse_vector(h);
        q.query_sparse_indices_ = std::move(idx_buf);
        q.query_sparse_values_ = std::move(val_buf);
      } else {
        // Dense vector: Ruby Array of floats
        Rice::Array arr(ruby_data);
        q.query_vector_ = serialize_float_array(arr);
      }
    });

  // --- GroupByVectorQuery ---
  Rice::define_class_under<zvec::GroupByVectorQuery>(m, "GroupByVectorQuery")
    .define_constructor(Rice::Constructor<zvec::GroupByVectorQuery>())
    .define_method("field_name", [](const zvec::GroupByVectorQuery& q) { return q.field_name_; })
    .define_method("field_name=", [](zvec::GroupByVectorQuery& q, std::string v) { q.field_name_ = std::move(v); })
    .define_method("filter", [](const zvec::GroupByVectorQuery& q) { return q.filter_; })
    .define_method("filter=", [](zvec::GroupByVectorQuery& q, std::string v) { q.filter_ = std::move(v); })
    .define_method("include_vector?", [](const zvec::GroupByVectorQuery& q) { return q.include_vector_; })
    .define_method("include_vector=", [](zvec::GroupByVectorQuery& q, bool v) { q.include_vector_ = v; })
    .define_method("group_by_field_name", [](const zvec::GroupByVectorQuery& q) { return q.group_by_field_name_; })
    .define_method("group_by_field_name=", [](zvec::GroupByVectorQuery& q, std::string v) { q.group_by_field_name_ = std::move(v); })
    .define_method("group_count", [](const zvec::GroupByVectorQuery& q) { return q.group_count_; })
    .define_method("group_count=", [](zvec::GroupByVectorQuery& q, uint32_t v) { q.group_count_ = v; })
    .define_method("group_topk", [](const zvec::GroupByVectorQuery& q) { return q.group_topk_; })
    .define_method("group_topk=", [](zvec::GroupByVectorQuery& q, uint32_t v) { q.group_topk_ = v; })
    .define_method("query_params", [](const zvec::GroupByVectorQuery& q) { return q.query_params_; })
    .define_method("query_params=", [](zvec::GroupByVectorQuery& q, zvec::QueryParams::Ptr p) { q.query_params_ = p; })
    .define_method("set_vector", [](zvec::GroupByVectorQuery& q,
                                    const zvec::FieldSchema& fs,
                                    Rice::Object ruby_data) {
      auto dt = fs.data_type();
      if (zvec::FieldSchema::is_sparse_vector_field(dt)) {
        Rice::Hash h(ruby_data);
        auto [idx_buf, val_buf] = serialize_sparse_vector(h);
        q.query_sparse_indices_ = std::move(idx_buf);
        q.query_sparse_values_ = std::move(val_buf);
      } else {
        Rice::Array arr(ruby_data);
        q.query_vector_ = serialize_float_array(arr);
      }
    });

  // --- GroupResult ---
  Rice::define_class_under<zvec::GroupResult>(m, "GroupResult")
    .define_method("group_by_value", [](const zvec::GroupResult& gr) { return gr.group_by_value_; })
    .define_method("docs", [](const zvec::GroupResult& gr) {
      Rice::Array result;
      for (const auto& doc : gr.docs_) {
        // Copy doc into a shared_ptr for Ruby ownership
        auto ptr = std::make_shared<zvec::Doc>(doc);
        result.push(ptr);
      }
      return result;
    });
}
