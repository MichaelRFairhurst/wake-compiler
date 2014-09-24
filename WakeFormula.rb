require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.4.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "1660eafc0624b0b5459172ae30acc5fcbe8ad342"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

