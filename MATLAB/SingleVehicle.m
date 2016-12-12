%SingleVehicle;

% Compute position and velocity of a vehicle following 
% a relative-position/relative-velocity controller
% between two constant velocity vehicles

% controller: a = -bb(x - xb) - bf(x - xf) -c*bb(v-vb) - c*bf(v-vf) 
% 2nd order system with zeta = c/2*sqrt(bb+bf)

syms bb bf c;

A = [0 1; -(bb + bf), -c*(bb+bf)];
B = [0 0 0 0; bb, c*bb, bf, c*bf];
C = eye(2);
D = [0 0 0 0; 0 0 0 0];

syms s;

%Average vehicle distance and velocity
L = 2; %m
v = 0.06; %m/s


%Init condition
x0 = [0; 0.5*v];

bb = 2;
bf = 2.000000001;
c = 0.5;

A = double(subs(A));
B = double(subs(B));


%Transfer matrix
sys = ss(A, B, C, D);

t = 0:0.01:10;

%Input
u1 = -L + v*t';
u2 = v*t'./t';
u3 = L + v*t';
u4 = v*t'./t';

u = [u1, u2, u3, u4];
u(1,2) = v;
u(1,4) = v;

[y,t,x] = lsim(sys, u, t, x0);

 

%y1 = subs(y, bb,bb1);
%y1 = subs(y1, bf,bf1);
%y1 = subs(y1, c,c1);

%y1 = simplify(y1);

plot(t,y(:,1) + L - v*t);
%plot(t,y(:,2));

