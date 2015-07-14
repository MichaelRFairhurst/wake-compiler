require "formula"

class Wake < Formula
  url "https://github.com/MichaelRFairhurst/wake-compiler/archive/v0.2.1.tar.gz"
  homepage "http://www.wakelang.com"
  sha1 "5488504a699937adc181c023e193ef14170cb0dc"

  depends_on "boost" => 'c++11'
  depends_on "flex"
  depends_on "bison"

  def install
    system "make", "install", "TEST=false", "prefix=#{prefix}"
  end
end

