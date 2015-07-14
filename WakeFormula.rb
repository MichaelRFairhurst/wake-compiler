require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/releases/download/v0.2.1/v0.2.1.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "ee1a14e2c84ce547d68d62cb0da51af3defdf9c7"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

