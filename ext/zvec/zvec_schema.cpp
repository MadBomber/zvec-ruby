#include "zvec_common.hpp"

using namespace Rice;

static inline Rice::Object rb_nil() { return Rice::Object(Qnil); }

void init_zvec_schema(Rice::Module& m) {
  // --- FieldSchema ---
  Rice::define_class_under<zvec::FieldSchema>(m, "FieldSchema")
    .define_constructor(Rice::Constructor<zvec::FieldSchema, const std::string&, zvec::DataType>(),
      Rice::Arg("name"), Rice::Arg("data_type"))
    .define_method("name", &zvec::FieldSchema::name)
    .define_method("data_type", &zvec::FieldSchema::data_type)
    .define_method("dimension", &zvec::FieldSchema::dimension)
    .define_method("dimension=", &zvec::FieldSchema::set_dimension)
    .define_method("nullable?", &zvec::FieldSchema::nullable)
    .define_method("nullable=", &zvec::FieldSchema::set_nullable)
    .define_method("vector_field?", static_cast<bool (zvec::FieldSchema::*)() const>(&zvec::FieldSchema::is_vector_field))
    .define_method("dense_vector?", &zvec::FieldSchema::is_dense_vector)
    .define_method("sparse_vector?", &zvec::FieldSchema::is_sparse_vector)
    .define_method("array_type?", &zvec::FieldSchema::is_array_type)
    .define_method("invert_index?", &zvec::FieldSchema::has_invert_index)
    .define_method("index_type", &zvec::FieldSchema::index_type)
    .define_method("index_params", &zvec::FieldSchema::index_params)
    .define_method("index_params=", static_cast<void (zvec::FieldSchema::*)(const zvec::IndexParams::Ptr&)>(&zvec::FieldSchema::set_index_params))
    .define_method("to_s", &zvec::FieldSchema::to_string)
    // Static factory that accepts keyword-style args for convenience
    .define_singleton_function("create", [](std::string name, zvec::DataType data_type,
                                            uint32_t dimension, bool nullable,
                                            Rice::Object index_params) -> zvec::FieldSchema::Ptr {
      zvec::IndexParams::Ptr ip;
      if (!index_params.is_nil()) {
        // Extract base class reference and clone to get a shared_ptr
        auto& params = Rice::detail::From_Ruby<zvec::IndexParams&>().convert(index_params.value());
        ip = params.clone();
      }
      return std::make_shared<zvec::FieldSchema>(name, data_type, dimension, nullable, ip);
    },
      Rice::Arg("name"),
      Rice::Arg("data_type"),
      Rice::Arg("dimension") = (uint32_t)0,
      Rice::Arg("nullable") = false,
      Rice::Arg("index_params") = Rice::Object(Qnil));

  // --- CollectionSchema ---
  Rice::define_class_under<zvec::CollectionSchema>(m, "CollectionSchema")
    .define_constructor(Rice::Constructor<zvec::CollectionSchema, const std::string&>(),
      Rice::Arg("name"))
    .define_method("name", &zvec::CollectionSchema::name)
    .define_method("has_field?", &zvec::CollectionSchema::has_field)
    .define_method("to_s", &zvec::CollectionSchema::to_string)
    .define_method("all_field_names", &zvec::CollectionSchema::all_field_names)
    .define_method("add_field", &zvec::CollectionSchema::add_field)
    // fields — returns Array of FieldSchema::Ptr
    .define_method("fields", [](const zvec::CollectionSchema& cs) {
      auto flds = cs.fields();
      Rice::Array result;
      for (auto& f : flds) result.push(f);
      return result;
    })
    .define_method("forward_fields", [](const zvec::CollectionSchema& cs) {
      auto flds = cs.forward_fields();
      Rice::Array result;
      for (auto& f : flds) result.push(f);
      return result;
    })
    .define_method("vector_fields", [](const zvec::CollectionSchema& cs) {
      auto flds = cs.vector_fields();
      Rice::Array result;
      for (auto& f : flds) result.push(f);
      return result;
    })
    .define_method("get_field", [](const zvec::CollectionSchema& cs, const std::string& name) -> Rice::Object {
      const zvec::FieldSchema* fs = cs.get_field(name);
      if (!fs) return rb_nil();
      // Return a copy wrapped in shared_ptr
      return Rice::detail::To_Ruby<zvec::FieldSchema::Ptr>().convert(
        std::make_shared<zvec::FieldSchema>(*fs));
    })
    // Convenience: create schema with an array of FieldSchema::Ptr
    .define_singleton_function("create", [](std::string name, Rice::Array fields) {
      zvec::FieldSchemaPtrList flds;
      flds.reserve(fields.size());
      for (size_t i = 0; i < fields.size(); i++) {
        flds.push_back(Rice::detail::From_Ruby<zvec::FieldSchema::Ptr>().convert(fields[i].value()));
      }
      return zvec::CollectionSchema(name, flds);
    },
      Rice::Arg("name"), Rice::Arg("fields"));

  // --- CollectionStats ---
  Rice::define_class_under<zvec::CollectionStats>(m, "CollectionStats")
    .define_method("doc_count", [](const zvec::CollectionStats& s) { return s.doc_count; })
    .define_method("index_completeness", [](const zvec::CollectionStats& s) {
      Rice::Hash result;
      for (const auto& [k, v] : s.index_completeness) {
        result[Rice::String(k)] = v;
      }
      return result;
    })
    .define_method("to_s", &zvec::CollectionStats::to_string);
}
