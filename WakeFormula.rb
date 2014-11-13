require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.0.6.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "adf407afef17ffb81214e77be01b478442ddc81c"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

