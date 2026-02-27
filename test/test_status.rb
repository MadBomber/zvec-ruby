# frozen_string_literal: true

require "test_helper"

class TestStatus < Minitest::Test
  def test_ok_status
    s = Zvec::Status.new
    assert s.ok?
    assert_equal Zvec::StatusCode::OK, s.code
    assert_equal "OK", s.to_s
  end

  def test_exception_hierarchy
    assert Zvec::Error < StandardError
    assert Zvec::NotFoundError < Zvec::Error
    assert Zvec::AlreadyExistsError < Zvec::Error
    assert Zvec::InvalidArgumentError < Zvec::Error
    assert Zvec::PermissionDeniedError < Zvec::Error
    assert Zvec::FailedPreconditionError < Zvec::Error
    assert Zvec::ResourceExhaustedError < Zvec::Error
    assert Zvec::UnavailableError < Zvec::Error
    assert Zvec::InternalError < Zvec::Error
    assert Zvec::NotSupportedError < Zvec::Error
  end
end
