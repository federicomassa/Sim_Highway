%TwoVehicles;

% Compute position and velocity of two vehicles following 
% a relative-position/relative-velocity controller
% between two constant velocity vehicles

% controller: a = -bb(x - xb) - bf(x - xf) -c*bb(v-vb) - c*bf(v-vf) 
syms bb bf c;

A = [0 1 0 0; -(bb + bf), -c*(bb+bf), bf, c*bf; 0 0 0 1; bb, c*bb, -(bb+bf), -c*(bb+bf)];
B = [0 0 0 0; bb, c*bb, 0, 0; 0 0 0 0; 0, 0, bf, c*bf];
C = eye(4);
D = zeros(4);

syms s;

%Average vehicle distance and velocity
L = 20; %m
v = 20; %m/s


%Init condition
x0 = [0; v; L; v];

bb = 2;
bf = 3;
c = 1;

A = double(subs(A));
B = double(subs(B));


%Transfer matrix
sys = ss(A, B, C, D);

t = 0:0.01:10;

%Input
u1 = -L + v*t';
u2 = v*t'./t';
u3 = 2*L + v*t';
u4 = v*t'./t';

u = [u1, u2, u3, u4];
u(1,2) = v;
u(1,4) = v;

[y,t,x] = lsim(sys, u, t, x0);

 

%y1 = subs(y, bb,bb1);
%y1 = subs(y1, bf,bf1);
%y1 = subs(y1, c,c1);

%y1 = simplify(y1);

plot(t,y(:,1) + L - v*t, t,y(:,3) - y(:,1), t, 2*L + v*t - y(:,3));

