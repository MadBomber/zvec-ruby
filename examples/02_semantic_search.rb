#!/usr/bin/env ruby
# frozen_string_literal: true

# Semantic search with real text embeddings using Informers + Zvec.
#
# This example demonstrates:
#   1. Loading markdown documents from disk
#   2. Generating 384-dim embeddings with all-MiniLM-L6-v2
#   3. Storing documents and vectors in a zvec collection
#   4. Querying by natural language and seeing ranked results
#
# First run downloads the model (~23 MB). Subsequent runs use cache.
#
# Run:  bundle exec ruby examples/02_semantic_search.rb

require "bundler/setup"
require "zvec"
require "informers"
require "tmpdir"

DIM = 384  # all-MiniLM-L6-v2 output dimensionality

DOCS_DIR = File.expand_path("docs", __dir__)

# ── 1. Load markdown documents ───────────────────────────────────────

docs = Dir.glob(File.join(DOCS_DIR, "*.md")).sort.map do |path|
  { filename: File.basename(path), content: File.read(path) }
end

puts "Loaded #{docs.size} documents:"
docs.each { |d| puts "  #{d[:filename]}" }

# ── 2. Generate embeddings ───────────────────────────────────────────

puts "\nInitializing embedding model (all-MiniLM-L6-v2)..."
model = Informers.pipeline("embedding", "sentence-transformers/all-MiniLM-L6-v2")

puts "Generating embeddings..."
docs.each do |doc|
  doc[:embedding] = model.(doc[:content])
end
puts "Done. Each embedding has #{docs.first[:embedding].size} dimensions."

# ── 3. Define schema and create collection ───────────────────────────

pk_field       = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
filename_field = Zvec::FieldSchema.create("filename", Zvec::DataType::STRING)
content_field  = Zvec::FieldSchema.create("content", Zvec::DataType::STRING)
embedding_field = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
                   dimension:    DIM,
                   index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))

schema = Zvec::CollectionSchema.create("docs",
           [pk_field, filename_field, content_field, embedding_field])

Dir.mktmpdir("zvec_semantic") do |dir|
  col_path = File.join(dir, "docs")
  col      = Zvec::Collection.create_and_open(col_path, schema)

  # ── 4. Insert documents ─────────────────────────────────────────────

  zvec_docs = docs.map.with_index do |d, i|
    doc = Zvec::Doc.new
    pk  = "doc#{i}"
    doc.pk = pk
    doc.set_field("pk",        Zvec::DataType::STRING,      pk)
    doc.set_field("filename",  Zvec::DataType::STRING,      d[:filename])
    doc.set_field("content",   Zvec::DataType::STRING,      d[:content])
    doc.set_field("embedding", Zvec::DataType::VECTOR_FP32, d[:embedding])
    doc
  end

  statuses = col.insert(zvec_docs)
  statuses.each_with_index do |s, i|
    puts "  Insert #{docs[i][:filename]}: #{s}"
  end
  col.flush

  puts "\nCollection has #{col.stats.doc_count} documents."

  # ── 5. Query by natural language ───────────────────────────────────

  queries = [
    "How do web servers handle requests?",
    "What are neural networks and deep learning?",
    "Tell me about the planets in our solar system",
    "How do I cook spaghetti al dente?",
    "What is object-oriented programming in Ruby?",
    "How does encryption protect internet traffic?",
  ]

  col_schema = col.schema

  queries.each do |query_text|
    query_vec = model.(query_text)
    results   = col.query_vector("embedding", query_vec, top_k: 3)

    puts "\n#{'─' * 60}"
    puts "Query: \"#{query_text}\""
    puts "#{'─' * 60}"
    results.each_with_index do |doc, rank|
      h = doc.to_h(col_schema)
      similarity = (1.0 - h['score']).round(4)
      puts "  #{rank + 1}. #{h['filename']}  (similarity: #{similarity})"
    end
  end

  col.flush
  puts "\nDone."
end
