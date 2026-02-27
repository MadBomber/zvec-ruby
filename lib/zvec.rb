# frozen_string_literal: true

require_relative "zvec/version"
require "zvec_ext"
require_relative "zvec/collection"
require_relative "zvec/doc"

module Zvec
  # Rice wraps shared_ptr<Collection> as Std::SharedPtr<zvec::Collection>,
  # a proxy class that delegates C++ methods via method_missing. Ruby
  # convenience methods must be included into this wrapper class, not
  # Zvec::Collection itself. We cannot use const_get because the wrapper
  # class name contains C++ mangled type parameters (e.g. angle brackets).
  def self.patch_shared_ptr_wrapper!
    ObjectSpace.each_object(Class) do |klass|
      if klass.name&.start_with?("Std::SharedPtr") && klass.name&.include?("Collection")
        klass.include(Zvec::CollectionConvenience)
        return klass
      end
    end
    nil
  end
end

Zvec.patch_shared_ptr_wrapper!
