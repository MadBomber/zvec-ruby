# Semantic Search

**Source:** [`examples/02_semantic_search.rb`](https://github.com/MadBomber/zvec/blob/main/examples/02_semantic_search.rb)

This example demonstrates realistic semantic search by using the [informers](https://github.com/ankane/informers) gem to generate real text embeddings from markdown documents, store them in zvec, and query by natural language.

## Dependencies

The `informers` gem (which pulls in `onnxruntime` and `tokenizers`) is required. It's included in the project's `Gemfile` under the development group.

The example uses the `sentence-transformers/all-MiniLM-L6-v2` model, which produces 384-dimensional embeddings. The model (~23 MB) is auto-downloaded on first run and cached for subsequent runs.

## Documents

Five short markdown files in `examples/docs/` cover distinct topics:

| File | Topic |
|------|-------|
| `ruby_basics.md` | Ruby programming language |
| `http_protocol.md` | HTTP and web servers |
| `machine_learning.md` | ML and neural networks |
| `cooking_pasta.md` | Italian pasta cooking |
| `solar_system.md` | Planets and astronomy |

## Schema

384-dimensional FP32 vectors with HNSW cosine index:

```ruby
pk_field        = Zvec::FieldSchema.create("pk", Zvec::DataType::STRING)
filename_field  = Zvec::FieldSchema.create("filename", Zvec::DataType::STRING)
content_field   = Zvec::FieldSchema.create("content", Zvec::DataType::STRING)
embedding_field = Zvec::FieldSchema.create("embedding", Zvec::DataType::VECTOR_FP32,
                    dimension:    384,
                    index_params: Zvec::HnswIndexParams.new(Zvec::MetricType::COSINE))
```

## How It Works

1. Load all `.md` files from `examples/docs/`
2. Initialize the informers embedding pipeline
3. Generate a 384-dim embedding for each document's text
4. Create a zvec collection in a temp directory, insert all docs
5. For each natural language query, embed the query text and search for the top 3 matches
6. Display results with similarity scores (`1.0 - distance`)

## Sample Queries and Results

```
Query: "How do web servers handle requests?"
  1. http_protocol.md  (similarity: 0.6172)
  2. machine_learning.md  (similarity: 0.1089)
  3. ruby_basics.md  (similarity: 0.1059)

Query: "What are neural networks and deep learning?"
  1. machine_learning.md  (similarity: 0.6591)
  2. http_protocol.md  (similarity: 0.1692)
  3. solar_system.md  (similarity: 0.1075)

Query: "How do I cook spaghetti al dente?"
  1. cooking_pasta.md  (similarity: 0.6277)
  2. solar_system.md  (similarity: 0.0473)
  3. machine_learning.md  (similarity: 0.0106)
```

Each query correctly identifies the most relevant document as the top result.

## Running

```bash
bundle exec ruby examples/02_semantic_search.rb
```

First run downloads the model. Subsequent runs use the cached version.

## Adapting for Your Own Documents

To use this pattern with your own documents, change `DOCS_DIR` to point at your markdown files:

```ruby
DOCS_DIR = "/path/to/your/docs"
```

For longer documents, consider splitting them into chunks (paragraphs or sections) before embedding, since transformer models have a limited context window (~512 tokens for all-MiniLM-L6-v2).
