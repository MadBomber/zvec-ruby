#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <zvec/db/collection.h>
#include <zvec/db/config.h>
#include <zvec/db/doc.h>
#include <zvec/db/index_params.h>
#include <zvec/db/options.h>
#include <zvec/db/query_params.h>
#include <zvec/db/schema.h>
#include <zvec/db/stats.h>
#include <zvec/db/status.h>
#include <zvec/db/type.h>
#include <zvec/ailego/utility/float_helper.h>

namespace zvec_rb {

// Exception classes defined in zvec_status.cpp
extern Rice::Class rb_cError;
extern Rice::Class rb_cNotFoundError;
extern Rice::Class rb_cAlreadyExistsError;
extern Rice::Class rb_cInvalidArgumentError;
extern Rice::Class rb_cPermissionDeniedError;
extern Rice::Class rb_cFailedPreconditionError;
extern Rice::Class rb_cResourceExhaustedError;
extern Rice::Class rb_cUnavailableError;
extern Rice::Class rb_cInternalError;
extern Rice::Class rb_cNotSupportedError;

// Raise the appropriate Ruby exception for a non-OK Status
void throw_if_error(const zvec::Status& status);

// Unwrap a Result<T> — return value on success, raise Ruby exception on error
template <typename T>
T unwrap_result(const tl::expected<T, zvec::Status>& result) {
  if (result.has_value()) return result.value();
  throw_if_error(result.error());
  // unreachable, throw_if_error always raises
  throw std::runtime_error("unreachable: unwrap_result after throw_if_error");
}

}  // namespace zvec_rb

// Init functions for each binding file
void init_zvec_types(Rice::Module& m);
void init_zvec_status(Rice::Module& m);
void init_zvec_params(Rice::Module& m);
void init_zvec_schema(Rice::Module& m);
void init_zvec_doc(Rice::Module& m);
void init_zvec_collection(Rice::Module& m);
void init_zvec_config(Rice::Module& m);
