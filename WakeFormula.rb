require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.2.0.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "36d3510f1e75bf64af4ce3af3c2b360bd503b9f8"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

