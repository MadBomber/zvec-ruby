# frozen_string_literal: true

require "test_helper"

class TestZvec < Minitest::Test
  def test_version
    refute_nil Zvec::VERSION
  end

  def test_zvec_available
    assert Zvec.zvec_available?
  end
end
