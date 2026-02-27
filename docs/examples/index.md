# Examples

Working examples are in the `examples/` directory of the repository.

| Example | Description |
|---------|-------------|
| [Basic Usage](basic-usage.md) | Schema definition, CRUD operations, and block-form collections with hand-crafted vectors |
| [Semantic Search](semantic-search.md) | Real text embeddings over markdown documents using the `informers` gem |

## Running Examples

```bash
bundle exec ruby examples/01_basic_usage.rb
bundle exec ruby examples/02_semantic_search.rb
```

The semantic search example downloads a model (~23 MB) on first run. Subsequent runs use the cached model.
