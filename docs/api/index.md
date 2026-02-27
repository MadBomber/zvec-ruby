# API Reference

Complete reference for every class, method, and constant in the Zvec Ruby gem.

## Core Classes

| Class | Description |
|-------|-------------|
| [FieldSchema](field-schema.md) | Defines a single field's name, type, dimension, and index |
| [CollectionSchema](collection-schema.md) | Groups fields into a collection schema |
| [Doc](doc.md) | Typed key-value container for a single record |
| [Collection](collection.md) | Persistent on-disk collection with CRUD and query operations |

## Index and Query Parameters

| Class | Description |
|-------|-------------|
| [Index Parameters](index-params.md) | HNSW, IVF, Flat, and Inverted index configuration |
| [Query Parameters](query-params.md) | Search-time tuning for HNSW, IVF, and Flat indexes |
| [VectorQuery](vector-query.md) | Full-featured vector query builder |
| [CollectionOptions](collection-options.md) | Options for opening collections |

## Status and Configuration

| Class | Description |
|-------|-------------|
| [Status and Errors](status-and-errors.md) | Status codes and exception hierarchy |
| [Global Configuration](global-config.md) | Memory, threading, and logging settings |
| [Enums](enums.md) | DataType, IndexType, MetricType, QuantizeType, StatusCode, Operator |
