# frozen_string_literal: true

require "test_helper"

class TestSchema < Minitest::Test
  def test_field_schema_basic
    fs = Zvec::FieldSchema.new("name", Zvec::DataType::STRING)
    assert_equal "name", fs.name
    assert_equal Zvec::DataType::STRING, fs.data_type
    refute fs.nullable?
    refute fs.vector_field?
  end

  def test_field_schema_create_with_dimension
    fs = Zvec::FieldSchema.create("vec", Zvec::DataType::VECTOR_FP32,
      dimension: 128,
      index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))
    assert_equal 128, fs.dimension
    assert fs.vector_field?
    assert fs.dense_vector?
    refute fs.sparse_vector?
    assert_equal Zvec::IndexType::HNSW, fs.index_type
  end

  def test_field_schema_nullable
    fs = Zvec::FieldSchema.create("opt", Zvec::DataType::STRING, nullable: true)
    assert fs.nullable?
  end

  def test_collection_schema
    pk = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
    vec = Zvec::FieldSchema.create("vec", Zvec::DataType::VECTOR_FP32,
      dimension: 4,
      index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

    schema = Zvec::CollectionSchema.create("test_col", [pk, vec])
    assert_equal "test_col", schema.name
    assert schema.has_field?("pk")
    assert schema.has_field?("vec")
    refute schema.has_field?("nonexistent")

    names = schema.all_field_names
    assert_includes names, "pk"
    assert_includes names, "vec"

    fields = schema.fields
    assert_equal 2, fields.size

    vf = schema.vector_fields
    assert_equal 1, vf.size
  end

  def test_collection_schema_get_field
    pk = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
    schema = Zvec::CollectionSchema.create("test", [pk])

    field = schema.get_field("pk")
    assert_equal "pk", field.name

    assert_nil schema.get_field("missing")
  end
end
