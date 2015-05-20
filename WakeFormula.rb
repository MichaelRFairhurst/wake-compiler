require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.2.0.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "0fefebc5c5c28f2adcabc44cc908155d2c655fd3"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

