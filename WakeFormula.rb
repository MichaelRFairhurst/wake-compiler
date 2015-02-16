require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.1.1.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "2bf34e4db66790feb13c2e0ccde0e71baaf44d99"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

