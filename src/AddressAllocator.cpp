#include "AddressAllocator.h"

AddressAllocator::AddressAllocator() {
	stringgenerator = -1;
}

string AddressAllocator::allocate() {
	stringgenerator++;

	string retval;

	int numeral = stringgenerator;
	while(numeral >= 0) {
		int quotient = numeral / 52;
		int place = numeral % 52;

		if(quotient > 0) {
			numeral = quotient - 1;
		} else {
			numeral = -1;
		}

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
a 0
b 1
c 2
d 3
e 4
f 5
g 6
h 7
i 8
j 9
k 10
l 11
m 12
n 13
o 14
p 15
q 16
r 17
s 18
t 19
u 20
v 21
w 22
x 23
y 24
z 25
A 26
B 27
C 28
D 29
E 30
F 31
G 32
H 33
I 34
J 35
K 36
L 37
M 38
N 39
O 40
P 41
Q 42
R 43
S 44
T 45
U 46
V 47
W 48
X 49
Y 50
Z 51
aa 0 0
ab 0 1
ac 0 2
ad 0 3
ae 0 4
af 0 5
ag 0 6
ah 0 7
ai 0 8
aj 0 9
ak 0 10
al 0 11
am 0 12
an 0 13
ao 0 14
ap 0 15
aq 0 16
ar 0 17
as 0 18
at 0 19
au 0 20
av 0 21
aw 0 22
ax 0 23
ay 0 24
az 0 25
aA 0 26
aB 0 27
aC 0 28
aD 0 29
aE 0 30
aF 0 31
aG 0 32
aH 0 33
aI 0 34
aJ 0 35
aK 0 26
aL 0 37
aM 0 38
aN 0 39
aO 0 40
aP 0 41
aQ 0 42
aR 0 43
aS 0 44
aT 0 45
aU 0 46
aV 0 47
aW 0 48
aX 0 49
aY 0 50
aZ 0 51
ba 1 0
*/
