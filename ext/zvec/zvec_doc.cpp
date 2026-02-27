#include "zvec_common.hpp"

using namespace Rice;
using float16_t = zvec::ailego::Float16;

static inline Rice::Object rb_nil() { return Rice::Object(Qnil); }

// Set a field on a Doc using a DataType discriminator
static void doc_set_field(zvec::Doc& doc, const std::string& name,
                          zvec::DataType dt, Rice::Object value) {
  if (value.is_nil()) {
    doc.set_null(name);
    return;
  }

  switch (dt) {
    case zvec::DataType::BINARY:
    case zvec::DataType::STRING:
      doc.set<std::string>(name, Rice::detail::From_Ruby<std::string>().convert(value.value()));
      break;
    case zvec::DataType::BOOL:
      doc.set<bool>(name, Rice::detail::From_Ruby<bool>().convert(value.value()));
      break;
    case zvec::DataType::INT32:
      doc.set<int32_t>(name, Rice::detail::From_Ruby<int32_t>().convert(value.value()));
      break;
    case zvec::DataType::INT64:
      doc.set<int64_t>(name, Rice::detail::From_Ruby<int64_t>().convert(value.value()));
      break;
    case zvec::DataType::UINT32:
      doc.set<uint32_t>(name, Rice::detail::From_Ruby<uint32_t>().convert(value.value()));
      break;
    case zvec::DataType::UINT64:
      doc.set<uint64_t>(name, Rice::detail::From_Ruby<uint64_t>().convert(value.value()));
      break;
    case zvec::DataType::FLOAT:
      doc.set<float>(name, Rice::detail::From_Ruby<float>().convert(value.value()));
      break;
    case zvec::DataType::DOUBLE:
      doc.set<double>(name, Rice::detail::From_Ruby<double>().convert(value.value()));
      break;

    // Dense vectors
    case zvec::DataType::VECTOR_FP32: {
      Rice::Array arr(value);
      std::vector<float> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<float>().convert(arr[i].value());
      doc.set<std::vector<float>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_FP64: {
      Rice::Array arr(value);
      std::vector<double> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<double>().convert(arr[i].value());
      doc.set<std::vector<double>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_FP16: {
      Rice::Array arr(value);
      std::vector<float16_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = float16_t(Rice::detail::From_Ruby<float>().convert(arr[i].value()));
      doc.set<std::vector<float16_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_INT8: {
      Rice::Array arr(value);
      std::vector<int8_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = static_cast<int8_t>(Rice::detail::From_Ruby<int>().convert(arr[i].value()));
      doc.set<std::vector<int8_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_INT16: {
      Rice::Array arr(value);
      std::vector<int16_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = static_cast<int16_t>(Rice::detail::From_Ruby<int>().convert(arr[i].value()));
      doc.set<std::vector<int16_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_BINARY32: {
      Rice::Array arr(value);
      std::vector<uint32_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<uint32_t>().convert(arr[i].value());
      doc.set<std::vector<uint32_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_BINARY64: {
      Rice::Array arr(value);
      std::vector<uint64_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<uint64_t>().convert(arr[i].value());
      doc.set<std::vector<uint64_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::VECTOR_INT4: {
      Rice::Array arr(value);
      std::vector<int8_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = static_cast<int8_t>(Rice::detail::From_Ruby<int>().convert(arr[i].value()));
      doc.set<std::vector<int8_t>>(name, std::move(vec));
      break;
    }

    // Sparse vectors
    case zvec::DataType::SPARSE_VECTOR_FP32: {
      Rice::Hash h(value);
      std::vector<uint32_t> indices;
      std::vector<float> values;
      for (auto it = h.begin(); it != h.end(); ++it) {
        indices.push_back(Rice::detail::From_Ruby<uint32_t>().convert((*it).first.value()));
        values.push_back(Rice::detail::From_Ruby<float>().convert((*it).second.value()));
      }
      doc.set<std::pair<std::vector<uint32_t>, std::vector<float>>>(
        name, {std::move(indices), std::move(values)});
      break;
    }
    case zvec::DataType::SPARSE_VECTOR_FP16: {
      Rice::Hash h(value);
      std::vector<uint32_t> indices;
      std::vector<float16_t> values;
      for (auto it = h.begin(); it != h.end(); ++it) {
        indices.push_back(Rice::detail::From_Ruby<uint32_t>().convert((*it).first.value()));
        values.push_back(float16_t(Rice::detail::From_Ruby<float>().convert((*it).second.value())));
      }
      doc.set<std::pair<std::vector<uint32_t>, std::vector<float16_t>>>(
        name, {std::move(indices), std::move(values)});
      break;
    }

    // Array types
    case zvec::DataType::ARRAY_STRING: {
      Rice::Array arr(value);
      std::vector<std::string> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<std::string>().convert(arr[i].value());
      doc.set<std::vector<std::string>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_INT32: {
      Rice::Array arr(value);
      std::vector<int32_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<int32_t>().convert(arr[i].value());
      doc.set<std::vector<int32_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_INT64: {
      Rice::Array arr(value);
      std::vector<int64_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<int64_t>().convert(arr[i].value());
      doc.set<std::vector<int64_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_UINT32: {
      Rice::Array arr(value);
      std::vector<uint32_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<uint32_t>().convert(arr[i].value());
      doc.set<std::vector<uint32_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_UINT64: {
      Rice::Array arr(value);
      std::vector<uint64_t> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<uint64_t>().convert(arr[i].value());
      doc.set<std::vector<uint64_t>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_FLOAT: {
      Rice::Array arr(value);
      std::vector<float> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<float>().convert(arr[i].value());
      doc.set<std::vector<float>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_DOUBLE: {
      Rice::Array arr(value);
      std::vector<double> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<double>().convert(arr[i].value());
      doc.set<std::vector<double>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_BOOL: {
      Rice::Array arr(value);
      std::vector<bool> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<bool>().convert(arr[i].value());
      doc.set<std::vector<bool>>(name, std::move(vec));
      break;
    }
    case zvec::DataType::ARRAY_BINARY: {
      Rice::Array arr(value);
      std::vector<std::string> vec(arr.size());
      for (size_t i = 0; i < arr.size(); i++)
        vec[i] = Rice::detail::From_Ruby<std::string>().convert(arr[i].value());
      doc.set<std::vector<std::string>>(name, std::move(vec));
      break;
    }

    default:
      throw std::runtime_error("Unsupported DataType for set_field");
  }
}

// Get a field from a Doc using a DataType discriminator — returns Ruby Object or nil
static Rice::Object doc_get_field(const zvec::Doc& doc, const std::string& name,
                                  zvec::DataType dt) {
  if (!doc.has(name) || doc.is_null(name)) return rb_nil();

  switch (dt) {
    case zvec::DataType::BINARY:
    case zvec::DataType::STRING: {
      auto r = doc.get<std::string>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<std::string>().convert(*r));
    }
    case zvec::DataType::BOOL: {
      auto r = doc.get<bool>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<bool>().convert(*r));
    }
    case zvec::DataType::INT32: {
      auto r = doc.get<int32_t>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<int32_t>().convert(*r));
    }
    case zvec::DataType::INT64: {
      auto r = doc.get<int64_t>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<int64_t>().convert(*r));
    }
    case zvec::DataType::UINT32: {
      auto r = doc.get<uint32_t>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<uint32_t>().convert(*r));
    }
    case zvec::DataType::UINT64: {
      auto r = doc.get<uint64_t>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<uint64_t>().convert(*r));
    }
    case zvec::DataType::FLOAT: {
      auto r = doc.get<float>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<float>().convert(*r));
    }
    case zvec::DataType::DOUBLE: {
      auto r = doc.get<double>(name);
      if (!r) return rb_nil();
      return Rice::Object(Rice::detail::To_Ruby<double>().convert(*r));
    }

    // Dense vectors
    case zvec::DataType::VECTOR_FP32: {
      auto r = doc.get<std::vector<float>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::VECTOR_FP64: {
      auto r = doc.get<std::vector<double>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::VECTOR_FP16: {
      auto r = doc.get<std::vector<float16_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(static_cast<float>(v));
      return arr;
    }
    case zvec::DataType::VECTOR_INT8: {
      auto r = doc.get<std::vector<int8_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(static_cast<int>(v));
      return arr;
    }
    case zvec::DataType::VECTOR_INT16: {
      auto r = doc.get<std::vector<int16_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(static_cast<int>(v));
      return arr;
    }
    case zvec::DataType::VECTOR_BINARY32: {
      auto r = doc.get<std::vector<uint32_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::VECTOR_BINARY64: {
      auto r = doc.get<std::vector<uint64_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::VECTOR_INT4: {
      auto r = doc.get<std::vector<int8_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(static_cast<int>(v));
      return arr;
    }

    // Sparse vectors
    case zvec::DataType::SPARSE_VECTOR_FP32: {
      auto r = doc.get<std::pair<std::vector<uint32_t>, std::vector<float>>>(name);
      if (!r) return rb_nil();
      Rice::Hash h;
      for (size_t i = 0; i < r->first.size(); i++) {
        h[Rice::detail::To_Ruby<uint32_t>().convert(r->first[i])] = r->second[i];
      }
      return h;
    }
    case zvec::DataType::SPARSE_VECTOR_FP16: {
      auto r = doc.get<std::pair<std::vector<uint32_t>, std::vector<float16_t>>>(name);
      if (!r) return rb_nil();
      Rice::Hash h;
      for (size_t i = 0; i < r->first.size(); i++) {
        h[Rice::detail::To_Ruby<uint32_t>().convert(r->first[i])] = static_cast<float>(r->second[i]);
      }
      return h;
    }

    // Array types
    case zvec::DataType::ARRAY_STRING:
    case zvec::DataType::ARRAY_BINARY: {
      auto r = doc.get<std::vector<std::string>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (const auto& v : *r) arr.push(Rice::String(v));
      return arr;
    }
    case zvec::DataType::ARRAY_INT32: {
      auto r = doc.get<std::vector<int32_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_INT64: {
      auto r = doc.get<std::vector<int64_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_UINT32: {
      auto r = doc.get<std::vector<uint32_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_UINT64: {
      auto r = doc.get<std::vector<uint64_t>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_FLOAT: {
      auto r = doc.get<std::vector<float>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_DOUBLE: {
      auto r = doc.get<std::vector<double>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(v);
      return arr;
    }
    case zvec::DataType::ARRAY_BOOL: {
      auto r = doc.get<std::vector<bool>>(name);
      if (!r) return rb_nil();
      Rice::Array arr;
      for (auto v : *r) arr.push(static_cast<bool>(v));
      return arr;
    }

    default:
      return rb_nil();
  }
}

void init_zvec_doc(Rice::Module& m) {
  Rice::define_class_under<zvec::Doc>(m, "Doc")
    .define_constructor(Rice::Constructor<zvec::Doc>())
    // Metadata
    .define_method("pk", &zvec::Doc::pk)
    .define_method("pk=", &zvec::Doc::set_pk)
    .define_method("score", &zvec::Doc::score)
    .define_method("score=", &zvec::Doc::set_score)
    .define_method("doc_id", &zvec::Doc::doc_id)
    .define_method("doc_id=", &zvec::Doc::set_doc_id)
    .define_method("operator", &zvec::Doc::get_operator)
    .define_method("operator=", &zvec::Doc::set_operator)
    // Field inspection
    .define_method("field_names", &zvec::Doc::field_names)
    .define_method("has_field?", &zvec::Doc::has)
    .define_method("has_value?", &zvec::Doc::has_value)
    .define_method("null?", &zvec::Doc::is_null)
    .define_method("empty?", &zvec::Doc::is_empty)
    .define_method("remove_field", &zvec::Doc::remove)
    .define_method("clear", &zvec::Doc::clear)
    .define_method("to_s", &zvec::Doc::to_string)
    // Type-dispatched set/get
    .define_method("set_field", [](zvec::Doc& doc, const std::string& name,
                                   zvec::DataType dt, Rice::Object value) {
      doc_set_field(doc, name, dt, value);
    })
    .define_method("get_field", [](const zvec::Doc& doc, const std::string& name,
                                   zvec::DataType dt) -> Rice::Object {
      return doc_get_field(doc, name, dt);
    })
    // Convenience: set_field using FieldSchema for type dispatch
    .define_method("set_field_by_schema", [](zvec::Doc& doc, const std::string& name,
                                             const zvec::FieldSchema& fs,
                                             Rice::Object value) {
      doc_set_field(doc, name, fs.data_type(), value);
    });
}
