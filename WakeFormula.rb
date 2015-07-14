require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.2.1.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "04920699e52f87e73f34f1a435f49bc3cbed9166"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

