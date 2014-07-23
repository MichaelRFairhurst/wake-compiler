require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.3.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "e221d97b3e4627a32e5be954ed8985a1e53c2958"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

