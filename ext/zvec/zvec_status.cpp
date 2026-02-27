#include "zvec_common.hpp"

namespace zvec_rb {

// Exception class storage
Rice::Class rb_cError;
Rice::Class rb_cNotFoundError;
Rice::Class rb_cAlreadyExistsError;
Rice::Class rb_cInvalidArgumentError;
Rice::Class rb_cPermissionDeniedError;
Rice::Class rb_cFailedPreconditionError;
Rice::Class rb_cResourceExhaustedError;
Rice::Class rb_cUnavailableError;
Rice::Class rb_cInternalError;
Rice::Class rb_cNotSupportedError;

void throw_if_error(const zvec::Status& status) {
  if (status.ok()) return;

  VALUE klass;
  switch (status.code()) {
    case zvec::StatusCode::NOT_FOUND:
      klass = rb_cNotFoundError.value();
      break;
    case zvec::StatusCode::ALREADY_EXISTS:
      klass = rb_cAlreadyExistsError.value();
      break;
    case zvec::StatusCode::INVALID_ARGUMENT:
      klass = rb_cInvalidArgumentError.value();
      break;
    case zvec::StatusCode::PERMISSION_DENIED:
      klass = rb_cPermissionDeniedError.value();
      break;
    case zvec::StatusCode::FAILED_PRECONDITION:
      klass = rb_cFailedPreconditionError.value();
      break;
    case zvec::StatusCode::RESOURCE_EXHAUSTED:
      klass = rb_cResourceExhaustedError.value();
      break;
    case zvec::StatusCode::UNAVAILABLE:
      klass = rb_cUnavailableError.value();
      break;
    case zvec::StatusCode::INTERNAL_ERROR:
      klass = rb_cInternalError.value();
      break;
    case zvec::StatusCode::NOT_SUPPORTED:
      klass = rb_cNotSupportedError.value();
      break;
    default:
      klass = rb_cError.value();
      break;
  }

  rb_raise(klass, "%s", status.message().c_str());
}

}  // namespace zvec_rb

void init_zvec_status(Rice::Module& m) {
  // Exception hierarchy: Zvec::Error < StandardError, then subclasses
  zvec_rb::rb_cError = Rice::define_class_under(m, "Error", rb_eStandardError);

  zvec_rb::rb_cNotFoundError =
    Rice::define_class_under(m, "NotFoundError", zvec_rb::rb_cError);
  zvec_rb::rb_cAlreadyExistsError =
    Rice::define_class_under(m, "AlreadyExistsError", zvec_rb::rb_cError);
  zvec_rb::rb_cInvalidArgumentError =
    Rice::define_class_under(m, "InvalidArgumentError", zvec_rb::rb_cError);
  zvec_rb::rb_cPermissionDeniedError =
    Rice::define_class_under(m, "PermissionDeniedError", zvec_rb::rb_cError);
  zvec_rb::rb_cFailedPreconditionError =
    Rice::define_class_under(m, "FailedPreconditionError", zvec_rb::rb_cError);
  zvec_rb::rb_cResourceExhaustedError =
    Rice::define_class_under(m, "ResourceExhaustedError", zvec_rb::rb_cError);
  zvec_rb::rb_cUnavailableError =
    Rice::define_class_under(m, "UnavailableError", zvec_rb::rb_cError);
  zvec_rb::rb_cInternalError =
    Rice::define_class_under(m, "InternalError", zvec_rb::rb_cError);
  zvec_rb::rb_cNotSupportedError =
    Rice::define_class_under(m, "NotSupportedError", zvec_rb::rb_cError);

  // Status class binding
  Rice::define_class_under<zvec::Status>(m, "Status")
    .define_constructor(Rice::Constructor<zvec::Status>())
    .define_method("ok?", &zvec::Status::ok)
    .define_method("code", &zvec::Status::code)
    .define_method("message", &zvec::Status::message)
    .define_method("to_s", [](const zvec::Status& s) -> std::string {
      if (s.ok()) return "OK";
      return std::string("Error(") + std::to_string(static_cast<int>(s.code())) +
             "): " + s.message();
    });
}
