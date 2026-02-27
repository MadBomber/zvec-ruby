module Zvec
  class Doc
    # Convert doc fields to a Ruby Hash using a CollectionSchema for type dispatch
    def to_h(schema)
      result = {"pk" => pk, "score" => score}
      field_names.each do |name|
        fs = schema.get_field(name)
        next unless fs

        result[name] = get_field(name, fs.data_type)
      end
      result
    end
  end
end
