#include "zvec_common.hpp"

void init_zvec_types(Rice::Module& m) {
  // DataType enum
  Rice::define_enum_under<zvec::DataType>("DataType", m)
    .define_value("UNDEFINED", zvec::DataType::UNDEFINED)
    .define_value("BINARY", zvec::DataType::BINARY)
    .define_value("STRING", zvec::DataType::STRING)
    .define_value("BOOL", zvec::DataType::BOOL)
    .define_value("INT32", zvec::DataType::INT32)
    .define_value("INT64", zvec::DataType::INT64)
    .define_value("UINT32", zvec::DataType::UINT32)
    .define_value("UINT64", zvec::DataType::UINT64)
    .define_value("FLOAT", zvec::DataType::FLOAT)
    .define_value("DOUBLE", zvec::DataType::DOUBLE)
    .define_value("VECTOR_BINARY32", zvec::DataType::VECTOR_BINARY32)
    .define_value("VECTOR_BINARY64", zvec::DataType::VECTOR_BINARY64)
    .define_value("VECTOR_FP16", zvec::DataType::VECTOR_FP16)
    .define_value("VECTOR_FP32", zvec::DataType::VECTOR_FP32)
    .define_value("VECTOR_FP64", zvec::DataType::VECTOR_FP64)
    .define_value("VECTOR_INT4", zvec::DataType::VECTOR_INT4)
    .define_value("VECTOR_INT8", zvec::DataType::VECTOR_INT8)
    .define_value("VECTOR_INT16", zvec::DataType::VECTOR_INT16)
    .define_value("SPARSE_VECTOR_FP16", zvec::DataType::SPARSE_VECTOR_FP16)
    .define_value("SPARSE_VECTOR_FP32", zvec::DataType::SPARSE_VECTOR_FP32)
    .define_value("ARRAY_BINARY", zvec::DataType::ARRAY_BINARY)
    .define_value("ARRAY_STRING", zvec::DataType::ARRAY_STRING)
    .define_value("ARRAY_BOOL", zvec::DataType::ARRAY_BOOL)
    .define_value("ARRAY_INT32", zvec::DataType::ARRAY_INT32)
    .define_value("ARRAY_INT64", zvec::DataType::ARRAY_INT64)
    .define_value("ARRAY_UINT32", zvec::DataType::ARRAY_UINT32)
    .define_value("ARRAY_UINT64", zvec::DataType::ARRAY_UINT64)
    .define_value("ARRAY_FLOAT", zvec::DataType::ARRAY_FLOAT)
    .define_value("ARRAY_DOUBLE", zvec::DataType::ARRAY_DOUBLE);

  // IndexType enum
  Rice::define_enum_under<zvec::IndexType>("IndexType", m)
    .define_value("UNDEFINED", zvec::IndexType::UNDEFINED)
    .define_value("HNSW", zvec::IndexType::HNSW)
    .define_value("IVF", zvec::IndexType::IVF)
    .define_value("FLAT", zvec::IndexType::FLAT)
    .define_value("INVERT", zvec::IndexType::INVERT);

  // MetricType enum
  Rice::define_enum_under<zvec::MetricType>("MetricType", m)
    .define_value("UNDEFINED", zvec::MetricType::UNDEFINED)
    .define_value("L2", zvec::MetricType::L2)
    .define_value("IP", zvec::MetricType::IP)
    .define_value("COSINE", zvec::MetricType::COSINE)
    .define_value("MIPSL2", zvec::MetricType::MIPSL2);

  // QuantizeType enum
  Rice::define_enum_under<zvec::QuantizeType>("QuantizeType", m)
    .define_value("UNDEFINED", zvec::QuantizeType::UNDEFINED)
    .define_value("FP16", zvec::QuantizeType::FP16)
    .define_value("INT8", zvec::QuantizeType::INT8)
    .define_value("INT4", zvec::QuantizeType::INT4);

  // StatusCode enum
  Rice::define_enum_under<zvec::StatusCode>("StatusCode", m)
    .define_value("OK", zvec::StatusCode::OK)
    .define_value("NOT_FOUND", zvec::StatusCode::NOT_FOUND)
    .define_value("ALREADY_EXISTS", zvec::StatusCode::ALREADY_EXISTS)
    .define_value("INVALID_ARGUMENT", zvec::StatusCode::INVALID_ARGUMENT)
    .define_value("PERMISSION_DENIED", zvec::StatusCode::PERMISSION_DENIED)
    .define_value("FAILED_PRECONDITION", zvec::StatusCode::FAILED_PRECONDITION)
    .define_value("RESOURCE_EXHAUSTED", zvec::StatusCode::RESOURCE_EXHAUSTED)
    .define_value("UNAVAILABLE", zvec::StatusCode::UNAVAILABLE)
    .define_value("INTERNAL_ERROR", zvec::StatusCode::INTERNAL_ERROR)
    .define_value("NOT_SUPPORTED", zvec::StatusCode::NOT_SUPPORTED)
    .define_value("UNKNOWN", zvec::StatusCode::UNKNOWN);

  // Operator enum
  Rice::define_enum_under<zvec::Operator>("Operator", m)
    .define_value("INSERT", zvec::Operator::INSERT)
    .define_value("UPSERT", zvec::Operator::UPSERT)
    .define_value("UPDATE", zvec::Operator::UPDATE)
    .define_value("DELETE", zvec::Operator::DELETE);
}
