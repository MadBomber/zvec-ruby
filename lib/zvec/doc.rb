# frozen_string_literal: true

module Zvec
  class Doc
    # Convert doc fields to a Ruby Hash. The schema parameter is required because
    # the C++ layer stores fields as type-erased values — the FieldSchema's data_type
    # selects the correct deserialization path (get_field dispatches on it).
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
