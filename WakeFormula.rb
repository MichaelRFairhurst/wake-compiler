require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.5.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "dc1086833f8a224f84cebc0b6da44fe8a67ad9f7"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

