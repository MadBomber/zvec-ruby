require "test_helper"

class TestZvec < Minitest::Test
  def test_version
    refute_nil Zvec::VERSION
    assert_equal "0.0.2", Zvec::VERSION
  end

  def test_zvec_available
    assert Zvec.zvec_available?
  end
end
