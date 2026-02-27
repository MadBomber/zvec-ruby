# frozen_string_literal: true

require "test_helper"
require "tmpdir"
require "fileutils"

class TestCollection < Minitest::Test
  def make_schema
    pk = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
    vec = Zvec::FieldSchema.create("vec", Zvec::DataType::VECTOR_FP32,
      dimension: 4,
      index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))
    Zvec::CollectionSchema.create("test_col", [pk, vec])
  end

  def make_doc(pk, vec)
    doc = Zvec::Doc.new
    doc.pk = pk
    doc.set_field("pk", Zvec::DataType::STRING, pk)
    doc.set_field("vec", Zvec::DataType::VECTOR_FP32, vec)
    doc
  end

  def test_create_and_open
    Dir.mktmpdir("zvec") do |dir|
      col_path = File.join(dir, "col")
      col = Zvec::Collection.create_and_open(col_path, make_schema)
      assert col
      assert_equal col_path, col.path
      col.destroy!
    end
  end

  def test_insert_and_flush
    Dir.mktmpdir("zvec") do |dir|
      col = Zvec::Collection.create_and_open(File.join(dir, "col"), make_schema)

      doc = make_doc("doc1", [1.0, 0.0, 0.0, 0.0])
      results = col.insert([doc])
      assert_equal 1, results.size
      assert results.first.ok?, "Insert failed: #{results.first.to_s}"

      col.flush

      stats = col.stats
      assert_equal 1, stats.doc_count

      col.destroy!
    end
  end

  def test_query
    Dir.mktmpdir("zvec") do |dir|
      col = Zvec::Collection.create_and_open(File.join(dir, "col"), make_schema)
      schema = col.schema

      3.times do |i|
        vec = [0.0, 0.0, 0.0, 0.0]
        vec[i % 4] = 1.0
        col.insert([make_doc("doc#{i}", vec)])
      end
      col.flush

      vq = Zvec::VectorQuery.new
      vq.topk = 2
      vq.field_name = "vec"
      fs = schema.get_field("vec")
      vq.set_vector(fs, [1.0, 0.0, 0.0, 0.0])

      results = col.query(vq)
      assert results.size >= 1
      assert_equal "doc0", results.first.pk

      col.destroy!
    end
  end

  def test_fetch
    Dir.mktmpdir("zvec") do |dir|
      col = Zvec::Collection.create_and_open(File.join(dir, "col"), make_schema)

      col.insert([make_doc("fetchme", [1.0, 2.0, 3.0, 4.0])])
      col.flush

      fetched = col.fetch(["fetchme"])
      assert fetched.key?("fetchme")
      assert_equal "fetchme", fetched["fetchme"].pk

      col.destroy!
    end
  end

  def test_delete
    Dir.mktmpdir("zvec") do |dir|
      col = Zvec::Collection.create_and_open(File.join(dir, "col"), make_schema)

      col.insert([make_doc("to_delete", [1.0, 0.0, 0.0, 0.0])])
      col.flush

      results = col.delete(["to_delete"])
      assert_equal 1, results.size
      col.flush

      fetched = col.fetch(["to_delete"])
      assert_empty fetched

      col.destroy!
    end
  end

  def test_reopen_collection
    dir = Dir.mktmpdir("zvec")
    col_path = File.join(dir, "col")

    # Create, insert, flush, then release
    col = Zvec::Collection.create_and_open(col_path, make_schema)
    col.insert([make_doc("persist", [1.0, 1.0, 1.0, 1.0])])
    col.flush
    col = nil
    GC.start

    # Reopen and verify data persisted
    col2 = Zvec::Collection.open(col_path)
    stats = col2.stats
    assert_equal 1, stats.doc_count

    col2.destroy!
  ensure
    FileUtils.rm_rf(dir) if dir
  end

  def test_query_vector_convenience
    Dir.mktmpdir("zvec") do |dir|
      col = Zvec::Collection.create_and_open(File.join(dir, "col"), make_schema)

      col.insert([make_doc("conv", [0.0, 1.0, 0.0, 0.0])])
      col.flush

      results = col.query_vector("vec", [0.0, 1.0, 0.0, 0.0], top_k: 1)
      assert_equal 1, results.size
      assert_equal "conv", results.first.pk

      col.destroy!
    end
  end
end
