#include "AddressAllocator.h"

AddressAllocator::AddressAllocator() {
	stringgenerator = -1;
}

string AddressAllocator::allocate() {
	stringgenerator++;

	string retval;

	int numeral = stringgenerator;
	while(numeral >= 0) {
		int quotient = numeral > 52 ? 53 : 52;
		int place = numeral % quotient;
		numeral = numeral / quotient - 1;

		if(place > 25) place -= 58;
		place += 97;
		retval += static_cast<const char>(place);
	}

	return retval;
}

void AddressAllocator::deallocate() {
	stringgenerator--;
}

/*
a 1
b 2
c 3
d 4
e 5
f 6
g 7
h 8
i 9
j 10
k 11
l 12
m 13
n 14
o 15
p 16
q 17
r 18
s 19
t 20
u 21
v 22
w 23
x 24
y 25
z 26
A 27
B 28
C 29
D 30
E 31
F 32
G 33
H 34
I 35
J 36
K 37
L 38
M 39
N 40
O 41
P 42
Q 43
R 44
S 45
T 46
U 47
V 48
W 49
X 50
Y 51
Z 52
aa 53
ab 54
ac 55
ad 56
ae 57
af 58
ag 59
ah 60
ai 61
aj 62
ak 63
al 64
am 65
an 66
ao 67
ap 68
aq 69
ar 70
as 71
at 72
au 73
av 74
aw 75
ax 76
ay 77
az 78
aA 79
aB 80
aC 81
aD 82
aE 83
aF 84
aG 85
aH 86
aI 87
aJ 88
aK 89
aL 90
aM 91
aN 92
aO 93
aP 94
aQ 95
aR 96
aS 97
aT 98
aU 99
aV 100
aW 101
aX 102
aY 103
aZ 104
ba 105*/
