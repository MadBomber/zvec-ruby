#include "zvec_common.hpp"

using namespace Rice;

void init_zvec_collection(Rice::Module& m) {
  Rice::define_class_under<zvec::Collection>(m, "Collection")
    // Static factory: create_and_open
    .define_singleton_function("create_and_open", [](const std::string& path,
                                                      const zvec::CollectionSchema& schema,
                                                      Rice::Object opts_obj) -> zvec::Collection::Ptr {
      zvec::CollectionOptions opts;
      if (!opts_obj.is_nil()) {
        opts = Rice::detail::From_Ruby<zvec::CollectionOptions>().convert(opts_obj.value());
      }
      return zvec_rb::unwrap_result(zvec::Collection::CreateAndOpen(path, schema, opts));
    },
      Rice::Arg("path"),
      Rice::Arg("schema"),
      Rice::Arg("options") = Rice::Object(Qnil))

    // Static factory: open
    .define_singleton_function("open", [](const std::string& path,
                                          Rice::Object opts_obj) -> zvec::Collection::Ptr {
      zvec::CollectionOptions opts;
      if (!opts_obj.is_nil()) {
        opts = Rice::detail::From_Ruby<zvec::CollectionOptions>().convert(opts_obj.value());
      }
      return zvec_rb::unwrap_result(zvec::Collection::Open(path, opts));
    },
      Rice::Arg("path"),
      Rice::Arg("options") = Rice::Object(Qnil))

    // Metadata
    .define_method("path", [](zvec::Collection& c) {
      return zvec_rb::unwrap_result(c.Path());
    })
    .define_method("schema", [](zvec::Collection& c) {
      return zvec_rb::unwrap_result(c.Schema());
    })
    .define_method("stats", [](zvec::Collection& c) {
      return zvec_rb::unwrap_result(c.Stats());
    })
    .define_method("options", [](zvec::Collection& c) {
      return zvec_rb::unwrap_result(c.Options());
    })

    // Lifecycle
    .define_method("flush", [](zvec::Collection& c) {
      zvec_rb::throw_if_error(c.Flush());
    })
    .define_method("destroy!", [](zvec::Collection& c) {
      zvec_rb::throw_if_error(c.Destroy());
    })

    // DDL — index management
    .define_method("create_index", [](zvec::Collection& c,
                                      const std::string& column,
                                      zvec::IndexParams::Ptr params,
                                      int concurrency) {
      zvec::CreateIndexOptions opts{concurrency};
      zvec_rb::throw_if_error(c.CreateIndex(column, params, opts));
    },
      Rice::Arg("column"),
      Rice::Arg("params"),
      Rice::Arg("concurrency") = 0)

    .define_method("drop_index", [](zvec::Collection& c, const std::string& column) {
      zvec_rb::throw_if_error(c.DropIndex(column));
    })

    .define_method("optimize", [](zvec::Collection& c, int concurrency) {
      zvec::OptimizeOptions opts{concurrency};
      zvec_rb::throw_if_error(c.Optimize(opts));
    },
      Rice::Arg("concurrency") = 0)

    // DDL — column management
    .define_method("add_column", [](zvec::Collection& c,
                                    zvec::FieldSchema::Ptr fs,
                                    const std::string& expression,
                                    int concurrency) {
      zvec::AddColumnOptions opts{concurrency};
      zvec_rb::throw_if_error(c.AddColumn(fs, expression, opts));
    },
      Rice::Arg("field_schema"),
      Rice::Arg("expression") = std::string(""),
      Rice::Arg("concurrency") = 0)

    .define_method("drop_column", [](zvec::Collection& c, const std::string& name) {
      zvec_rb::throw_if_error(c.DropColumn(name));
    })

    .define_method("alter_column", [](zvec::Collection& c,
                                      const std::string& name,
                                      const std::string& rename,
                                      Rice::Object new_schema_obj,
                                      int concurrency) {
      zvec::FieldSchema::Ptr new_schema = nullptr;
      if (!new_schema_obj.is_nil()) {
        new_schema = Rice::detail::From_Ruby<zvec::FieldSchema::Ptr>().convert(new_schema_obj.value());
      }
      zvec::AlterColumnOptions opts{concurrency};
      zvec_rb::throw_if_error(c.AlterColumn(name, rename, new_schema, opts));
    },
      Rice::Arg("name"),
      Rice::Arg("rename") = std::string(""),
      Rice::Arg("new_schema") = Rice::Object(Qnil),
      Rice::Arg("concurrency") = 0)

    // DML — write operations
    .define_method("insert", [](zvec::Collection& c, Rice::Array ruby_docs) {
      std::vector<zvec::Doc> docs;
      docs.reserve(ruby_docs.size());
      for (size_t i = 0; i < ruby_docs.size(); i++) {
        docs.push_back(Rice::detail::From_Ruby<zvec::Doc>().convert(ruby_docs[i].value()));
      }
      auto results = zvec_rb::unwrap_result(c.Insert(docs));
      Rice::Array arr;
      for (const auto& s : results) {
        arr.push(Rice::Object(Rice::detail::To_Ruby<zvec::Status>().convert(zvec::Status(s))));
      }
      return arr;
    })

    .define_method("upsert", [](zvec::Collection& c, Rice::Array ruby_docs) {
      std::vector<zvec::Doc> docs;
      docs.reserve(ruby_docs.size());
      for (size_t i = 0; i < ruby_docs.size(); i++) {
        docs.push_back(Rice::detail::From_Ruby<zvec::Doc>().convert(ruby_docs[i].value()));
      }
      auto results = zvec_rb::unwrap_result(c.Upsert(docs));
      Rice::Array arr;
      for (const auto& s : results) {
        arr.push(Rice::Object(Rice::detail::To_Ruby<zvec::Status>().convert(zvec::Status(s))));
      }
      return arr;
    })

    .define_method("update", [](zvec::Collection& c, Rice::Array ruby_docs) {
      std::vector<zvec::Doc> docs;
      docs.reserve(ruby_docs.size());
      for (size_t i = 0; i < ruby_docs.size(); i++) {
        docs.push_back(Rice::detail::From_Ruby<zvec::Doc>().convert(ruby_docs[i].value()));
      }
      auto results = zvec_rb::unwrap_result(c.Update(docs));
      Rice::Array arr;
      for (const auto& s : results) {
        arr.push(Rice::Object(Rice::detail::To_Ruby<zvec::Status>().convert(zvec::Status(s))));
      }
      return arr;
    })

    .define_method("delete", [](zvec::Collection& c, Rice::Array ruby_pks) {
      std::vector<std::string> pks;
      pks.reserve(ruby_pks.size());
      for (size_t i = 0; i < ruby_pks.size(); i++) {
        pks.push_back(Rice::detail::From_Ruby<std::string>().convert(ruby_pks[i].value()));
      }
      auto results = zvec_rb::unwrap_result(c.Delete(pks));
      Rice::Array arr;
      for (const auto& s : results) {
        arr.push(Rice::Object(Rice::detail::To_Ruby<zvec::Status>().convert(zvec::Status(s))));
      }
      return arr;
    })

    .define_method("delete_by_filter", [](zvec::Collection& c, const std::string& filter) {
      zvec_rb::throw_if_error(c.DeleteByFilter(filter));
    })

    // DQL — query operations
    .define_method("query", [](zvec::Collection& c, const zvec::VectorQuery& vq) {
      auto docs = zvec_rb::unwrap_result(c.Query(vq));
      Rice::Array arr;
      for (auto& d : docs) {
        zvec::Doc copy(*d);
        arr.push(Rice::Object(Rice::detail::To_Ruby<zvec::Doc>().convert(std::move(copy))));
      }
      return arr;
    })

    .define_method("group_by_query", [](zvec::Collection& c, const zvec::GroupByVectorQuery& gq) {
      auto results = zvec_rb::unwrap_result(c.GroupByQuery(gq));
      Rice::Array arr;
      for (auto& gr : results) arr.push(gr);
      return arr;
    })

    .define_method("fetch", [](zvec::Collection& c, Rice::Array ruby_pks) -> Rice::Object {
      std::vector<std::string> pks;
      pks.reserve(ruby_pks.size());
      for (size_t i = 0; i < ruby_pks.size(); i++) {
        pks.push_back(Rice::detail::From_Ruby<std::string>().convert(ruby_pks[i].value()));
      }
      auto doc_map = zvec_rb::unwrap_result(c.Fetch(pks));
      VALUE rb_hash = rb_hash_new();
      for (auto& [k, v] : doc_map) {
        if (!v) continue;
        zvec::Doc doc_copy = *v;
        VALUE rb_key = Rice::detail::To_Ruby<std::string>().convert(k);
        VALUE rb_val = Rice::detail::To_Ruby<zvec::Doc>().convert(doc_copy);
        rb_hash_aset(rb_hash, rb_key, rb_val);
      }
      return Rice::Object(rb_hash);
    });
}
