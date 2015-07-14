require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/releases/download/v0.2.1/v0.2.1.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "c58934fac871b8e3345aa793e3543210d849cda4"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

