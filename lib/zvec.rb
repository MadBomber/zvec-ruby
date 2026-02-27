require_relative "zvec/version"
require "zvec_ext"
require_relative "zvec/collection"
require_relative "zvec/doc"

# Rice wraps shared_ptr<Collection> as Std::SharedPtr<zvec::Collection>,
# which delegates C++ methods via method_missing. Ruby convenience methods
# must be included into this wrapper class, not Zvec::Collection itself.
# We discover the wrapper class by checking known Std:: classes.
ObjectSpace.each_object(Class) do |klass|
  if klass.name&.start_with?("Std::SharedPtr") && klass.name&.include?("Collection")
    klass.include(Zvec::CollectionConvenience)
  end
end
