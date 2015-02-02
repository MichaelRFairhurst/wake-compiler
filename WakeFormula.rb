require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.1.0.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "abd360865160f23c42b2ea36d7b1952ef900f43c"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

