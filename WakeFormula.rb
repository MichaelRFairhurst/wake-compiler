require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.5.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "01f2bfc819273019e49f2a1552506ee1f5e5d9e0"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

