# frozen_string_literal: true

require "test_helper"

class TestParams < Minitest::Test
  def test_hnsw_index_params_defaults
    p = Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE)
    assert_equal 50, p.m
    assert_equal 500, p.ef_construction
    assert_equal Zvec::MetricType::COSINE, p.metric_type
    assert_equal Zvec::QuantizeType::UNDEFINED, p.quantize_type
    assert p.vector_index_type?
  end

  def test_hnsw_index_params_custom
    p = Zvec::HnswIndexParams.new(Zvec::MetricType::L2, 32, 200, Zvec::QuantizeType::FP16)
    assert_equal 32, p.m
    assert_equal 200, p.ef_construction
    assert_equal Zvec::QuantizeType::FP16, p.quantize_type
  end

  def test_flat_index_params
    p = Zvec::FlatIndexParams.new(Zvec::MetricType::IP)
    assert_equal Zvec::MetricType::IP, p.metric_type
    assert p.vector_index_type?
  end

  def test_ivf_index_params
    p = Zvec::IVFIndexParams.new(Zvec::MetricType::L2)
    assert_equal 1024, p.n_list
    assert_equal 10, p.n_iters
    refute p.use_soar?
  end

  def test_invert_index_params
    p = Zvec::InvertIndexParams.new
    assert p.enable_range_optimization?
    refute p.enable_extended_wildcard?
    refute p.vector_index_type?
  end

  def test_hnsw_query_params
    p = Zvec::HnswQueryParams.new
    assert_equal 300, p.ef
    assert_in_delta 0.0, p.radius
  end

  def test_ivf_query_params
    p = Zvec::IVFQueryParams.new(20)
    assert_equal 20, p.nprobe
  end

  def test_flat_query_params
    p = Zvec::FlatQueryParams.new
    assert_in_delta 10.0, p.scale_factor
  end

  def test_collection_options
    o = Zvec::CollectionOptions.new
    refute o.read_only?
    assert o.enable_mmap?
    assert o.max_buffer_size > 0
  end

  def test_vector_query
    vq = Zvec::VectorQuery.new
    vq.topk = 10
    vq.field_name = "embedding"
    vq.filter = "category = 'test'"
    vq.include_vector = true

    assert_equal 10, vq.topk
    assert_equal "embedding", vq.field_name
    assert_equal "category = 'test'", vq.filter
    assert vq.include_vector?
  end
end
