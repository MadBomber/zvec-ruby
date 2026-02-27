# frozen_string_literal: true

module Zvec
  module CollectionConvenience
    # Convenience: build a VectorQuery and execute it
    def query_vector(field_name, vector, top_k:, filter: nil, include_vector: false, query_params: nil, output_fields: nil)
      vq = Zvec::VectorQuery.new
      vq.topk = top_k
      vq.field_name = field_name
      vq.filter = filter if filter
      vq.include_vector = include_vector
      vq.query_params = query_params if query_params
      vq.output_fields = output_fields if output_fields

      schema_obj = schema
      fs = schema_obj.get_field(field_name)
      raise ArgumentError, "Unknown field: #{field_name}" unless fs

      vq.set_vector(fs, vector)
      query(vq)
    end
  end

  # Block-form open: yields the collection and flushes on block exit
  def self.open_collection(path, options: nil)
    col = Collection.open(path, options)
    yield col
  ensure
    col&.flush
  end
end
