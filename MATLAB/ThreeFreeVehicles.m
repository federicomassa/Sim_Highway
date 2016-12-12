syms bb bf c;

L = 20;
v = 20;

A = [0 1 0 0 0 0; 
    -bf, -c*bf, bf, c*bf, 0, 0;
    0 0 0 1 0 0;
    bb, c*bb, -(bb+bf), -c*(bb+bf), bf, c*bf;
    0 0 0 0 0 1;
    0, 0, bb, c*bb, -bb, -c*bb];

x0 = [0; v; L; v; 2*L; v];

syms s;

G = A*inv(s*eye(6) - A)*x0 + x0;

X = G/s;

bb = 1;
bf = 1;
c = 0.5;

X1 = subs(X);

x1 = ilaplace(X1);