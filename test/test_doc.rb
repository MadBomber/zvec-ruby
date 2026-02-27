require "test_helper"

class TestDoc < Minitest::Test
  def test_doc_creation
    doc = Zvec::Doc.new
    assert doc.empty?
    assert_equal "", doc.pk
  end

  def test_doc_pk
    doc = Zvec::Doc.new
    doc.pk = "doc1"
    assert_equal "doc1", doc.pk
  end

  def test_doc_string_field
    doc = Zvec::Doc.new
    doc.set_field("name", Zvec::DataType::STRING, "hello")
    assert doc.has_field?("name")
    assert doc.has_value?("name")
    assert_equal "hello", doc.get_field("name", Zvec::DataType::STRING)
  end

  def test_doc_int32_field
    doc = Zvec::Doc.new
    doc.set_field("age", Zvec::DataType::INT32, 42)
    assert_equal 42, doc.get_field("age", Zvec::DataType::INT32)
  end

  def test_doc_float_field
    doc = Zvec::Doc.new
    doc.set_field("score", Zvec::DataType::FLOAT, 3.14)
    assert_in_delta 3.14, doc.get_field("score", Zvec::DataType::FLOAT), 0.001
  end

  def test_doc_bool_field
    doc = Zvec::Doc.new
    doc.set_field("active", Zvec::DataType::BOOL, true)
    assert_equal true, doc.get_field("active", Zvec::DataType::BOOL)
  end

  def test_doc_vector_fp32
    doc = Zvec::Doc.new
    vec = [1.0, 2.0, 3.0, 4.0]
    doc.set_field("vec", Zvec::DataType::VECTOR_FP32, vec)
    result = doc.get_field("vec", Zvec::DataType::VECTOR_FP32)
    assert_equal 4, result.size
    result.each_with_index do |v, i|
      assert_in_delta vec[i], v, 0.001
    end
  end

  def test_doc_null_field
    doc = Zvec::Doc.new
    doc.set_field("val", Zvec::DataType::STRING, nil)
    assert doc.has_field?("val")
    assert doc.null?("val")
    refute doc.has_value?("val")
    assert_nil doc.get_field("val", Zvec::DataType::STRING)
  end

  def test_doc_remove_field
    doc = Zvec::Doc.new
    doc.set_field("name", Zvec::DataType::STRING, "test")
    doc.remove_field("name")
    refute doc.has_field?("name")
  end

  def test_doc_clear
    doc = Zvec::Doc.new
    doc.pk = "test"
    doc.set_field("name", Zvec::DataType::STRING, "val")
    doc.clear
    assert doc.empty?
  end

  def test_doc_field_names
    doc = Zvec::Doc.new
    doc.set_field("a", Zvec::DataType::STRING, "1")
    doc.set_field("b", Zvec::DataType::INT32, 2)
    names = doc.field_names
    assert_includes names, "a"
    assert_includes names, "b"
  end

  def test_doc_missing_field_returns_nil
    doc = Zvec::Doc.new
    assert_nil doc.get_field("nonexistent", Zvec::DataType::STRING)
  end
end
