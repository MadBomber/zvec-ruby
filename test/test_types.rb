# frozen_string_literal: true

require "test_helper"

class TestTypes < Minitest::Test
  def test_data_type_values
    assert_equal Zvec::DataType::STRING, Zvec::DataType::STRING
    assert_equal Zvec::DataType::VECTOR_FP32, Zvec::DataType::VECTOR_FP32
    refute_equal Zvec::DataType::STRING, Zvec::DataType::INT32
  end

  def test_data_type_includes_all_scalars
    %i[BINARY STRING BOOL INT32 INT64 UINT32 UINT64 FLOAT DOUBLE].each do |name|
      assert Zvec::DataType.const_defined?(name), "Missing DataType::#{name}"
    end
  end

  def test_data_type_includes_vectors
    %i[VECTOR_FP32 VECTOR_FP64 VECTOR_FP16 VECTOR_INT8 VECTOR_INT16
       VECTOR_BINARY32 VECTOR_BINARY64 VECTOR_INT4].each do |name|
      assert Zvec::DataType.const_defined?(name), "Missing DataType::#{name}"
    end
  end

  def test_data_type_includes_sparse
    %i[SPARSE_VECTOR_FP32 SPARSE_VECTOR_FP16].each do |name|
      assert Zvec::DataType.const_defined?(name), "Missing DataType::#{name}"
    end
  end

  def test_index_type_values
    %i[UNDEFINED HNSW IVF FLAT INVERT].each do |name|
      assert Zvec::IndexType.const_defined?(name), "Missing IndexType::#{name}"
    end
  end

  def test_metric_type_values
    %i[UNDEFINED L2 IP COSINE MIPSL2].each do |name|
      assert Zvec::MetricType.const_defined?(name), "Missing MetricType::#{name}"
    end
  end

  def test_quantize_type_values
    %i[UNDEFINED FP16 INT8 INT4].each do |name|
      assert Zvec::QuantizeType.const_defined?(name), "Missing QuantizeType::#{name}"
    end
  end

  def test_status_code_values
    %i[OK NOT_FOUND ALREADY_EXISTS INVALID_ARGUMENT PERMISSION_DENIED
       FAILED_PRECONDITION RESOURCE_EXHAUSTED UNAVAILABLE INTERNAL_ERROR
       NOT_SUPPORTED UNKNOWN].each do |name|
      assert Zvec::StatusCode.const_defined?(name), "Missing StatusCode::#{name}"
    end
  end

  def test_operator_values
    %i[INSERT UPSERT UPDATE DELETE].each do |name|
      assert Zvec::Operator.const_defined?(name), "Missing Operator::#{name}"
    end
  end
end
