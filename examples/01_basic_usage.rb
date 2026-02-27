#!/usr/bin/env ruby
# frozen_string_literal: true

# Basic usage of the Zvec Ruby gem — an embedded vector database.
#
# This example demonstrates:
#   1. Defining a schema with scalar and vector fields
#   2. Creating a collection on disk
#   3. Inserting documents
#   4. Querying by vector similarity
#   5. Fetching documents by primary key
#   6. Deleting documents
#   7. Using the block-form open_collection
#
# Run:  bundle exec ruby examples/01_basic_usage.rb

require "bundler/setup"
require "zvec"
require "tmpdir"

DIM = 4  # vector dimensionality for this demo

# ── 1. Define the schema ─────────────────────────────────────────────
#
# Every collection needs at least a primary-key field and one vector field.
# Scalar fields (STRING, INT32, FLOAT, etc.) can store metadata alongside
# the vectors.

pk_field    = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
title_field = Zvec::FieldSchema.create("title", Zvec::DataType::STRING)
year_field  = Zvec::FieldSchema.create("year", Zvec::DataType::INT32)
embedding   = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
                dimension:    DIM,
                index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("movies", [pk_field, title_field, year_field, embedding])

# ── Helper to build a Doc ─────────────────────────────────────────────

def make_doc(id, title, year, vector)
  doc = Zvec::Doc.new
  doc.pk = id
  doc.set_field("pk",        Zvec::DataType::STRING,      id)
  doc.set_field("title",     Zvec::DataType::STRING,      title)
  doc.set_field("year",      Zvec::DataType::INT32,       year)
  doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, vector)
  doc
end

# ── 2. Create and populate a collection ───────────────────────────────

Dir.mktmpdir("zvec_demo") do |dir|
  col_path = File.join(dir, "movies")
  col      = Zvec::Collection.create_and_open(col_path, schema)

  puts "Collection created at: #{col.path}"

  movies = [
    make_doc("mov1", "The Matrix",        1999, [0.9, 0.1, 0.0, 0.2]),
    make_doc("mov2", "Inception",          2010, [0.8, 0.3, 0.1, 0.1]),
    make_doc("mov3", "Interstellar",       2014, [0.7, 0.5, 0.3, 0.0]),
    make_doc("mov4", "The Grand Budapest", 2014, [0.1, 0.2, 0.9, 0.8]),
    make_doc("mov5", "Amélie",             2001, [0.0, 0.1, 0.8, 0.9]),
  ]

  # Insert returns a Status per document
  statuses = col.insert(movies)
  statuses.each_with_index do |s, i|
    puts "  Insert #{movies[i].pk}: #{s}"
  end

  # Flush writes buffered data to disk
  col.flush

  stats = col.stats
  puts "\nDocument count: #{stats.doc_count}"

  # ── 3. Query by vector similarity ────────────────────────────────────
  #
  # Find the 3 movies most similar to a query vector using the convenience
  # method query_vector (wraps VectorQuery for you).

  query_vec = [0.85, 0.2, 0.05, 0.15]  # something close to sci-fi
  results   = col.query_vector("embedding", query_vec, top_k: 3)
  col_schema = col.schema

  puts "\nTop 3 similar to query #{query_vec.inspect}:"
  results.each do |doc|
    h = doc.to_h(col_schema)
    puts "  #{h['pk']}  #{h['title']}  (#{h['year']})  score=#{h['score'].round(4)}"
  end

  # ── 4. Fetch by primary key ──────────────────────────────────────────

  fetched = col.fetch(["mov4", "mov5"])
  puts "\nFetched by PK:"
  fetched.each do |pk, doc|
    h = doc.to_h(col_schema)
    puts "  #{pk}: #{h['title']} (#{h['year']})"
  end

  # ── 5. Delete a document ─────────────────────────────────────────────

  del_statuses = col.delete(["mov1"])
  puts "\nDelete mov1: #{del_statuses.first}"
  col.flush

  remaining = col.fetch(["mov1"])
  puts "Fetch mov1 after delete: #{remaining.empty? ? 'not found' : 'still present'}"
  puts "Document count: #{col.stats.doc_count}"

  # ── 6. Block-form open_collection ────────────────────────────────────
  #
  # Zvec.open_collection opens an existing collection, yields it, and
  # automatically flushes when the block exits.

  col.flush
  col = nil
  GC.start  # release the original handle

  puts "\nReopening with Zvec.open_collection..."
  Zvec.open_collection(col_path) do |c|
    stats = c.stats
    puts "  Document count after reopen: #{stats.doc_count}"

    results = c.query_vector("embedding", [0.0, 0.1, 0.85, 0.9], top_k: 2)
    puts "  Top 2 'arthouse' matches:"
    results.each do |doc|
      h = doc.to_h(c.schema)
      puts "    #{h['title']} (#{h['year']})  score=#{h['score'].round(4)}"
    end
  end

  puts "\nDone."
end
