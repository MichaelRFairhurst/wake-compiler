#include "JavascriptPropertyMangler.h"

std::string wake::JavascriptPropertyMangler::mangle(std::string property) {
	std::string mangled;

	for(std::string::iterator it = property.begin(); it != property.end(); ++it)
	switch (*it) {
		case '_': mangled += "__"; break;
		case '(': mangled += "_a"; break;
		case ')': mangled += "_b"; break;
		case '[': mangled += "_c"; break;
		case ']': mangled += "_d"; break;
		case '~': mangled += "_e"; break;
		case ' ': mangled += "_f"; break;
		case '@': mangled += "_g"; break;
		case '#': mangled += "_h"; break;
		case '%': mangled += "_i"; break;
		case '!': mangled += "_j"; break;
		case '^': mangled += "_k"; break;
		case '&': mangled += "_l"; break;
		case '*': mangled += "_m"; break;
		case '{': mangled += "_n"; break;
		case '}': mangled += "_o"; break;
		case ';': mangled += "_p"; break;
		case '<': mangled += "_q"; break;
		case '>': mangled += "_r"; break;
		case '?': mangled += "_s"; break;
		case '+': mangled += "_t"; break;
		case '=': mangled += "_u"; break;
		case '|': mangled += "_v"; break;
		case '\\': mangled += "_w"; break;
		case '/': mangled += "_x"; break;
		case ',': mangled += "_y"; break;
		case '\'': mangled += "_za"; break;
		case '"': mangled += "_zb"; break;
		case '.': mangled += "_zc"; break;
		case ':': mangled += "_zd"; break;
		case '-': mangled += "_ze"; break;
		case '`': mangled += "_zf"; break;
		case '$': mangled += *it; break;
		case 0: break;

		default:
			if((*it > 64 && *it < 91) || (*it > 96 && *it < 123) || (*it > 47 && *it < 58)) {
				mangled += *it;
			} else {
				throw std::string("Cannot mangle char " + *it);
			}
	}

	return mangled;
}
