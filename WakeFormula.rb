require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.2.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "6322af40e819eccfa40fcf49d1b403d377d4c45b"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

