#include "zvec_common.hpp"
#include "zvec_ext.hpp"

void Init_zvec_ext() {
  Rice::Module rb_mZvec = Rice::define_module("Zvec");

  rb_mZvec.define_module_function("zvec_available?", []() -> bool {
    return true;
  });

  // Initialize all binding modules in dependency order
  init_zvec_types(rb_mZvec);
  init_zvec_status(rb_mZvec);
  init_zvec_params(rb_mZvec);
  init_zvec_schema(rb_mZvec);
  init_zvec_doc(rb_mZvec);
  init_zvec_collection(rb_mZvec);
  init_zvec_config(rb_mZvec);
}
