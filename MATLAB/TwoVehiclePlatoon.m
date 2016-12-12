%TwoVehiclePlatoon;

% controller: a_rear = -bf(x - xf + D) -c*bf(v-vf)
             %a_lead = -bb(x - xb - D) -c*bf(v - vb) - K(v - v_desired)

%state vars: xR = x1 + D, vR = v1 - vd,
%            xL = x2, vL = v2 - vd
             
syms bb bf c K;
vd = 0.95*0.06;

A = [0 1 0 0; -bf, -c*bf, bf, c*bf; 0 0 0 1; bb, c*bb, -bb, -c*bb - K];
B = [vd; 0; vd; 0];
C = eye(4);
D = [0; 0; 0; 0];

syms s;

%Average vehicle distance and velocity
L = 2;


%Init condition
x0 = [L+0.1; 0; L; 0];

bb = 0.005;
bf = 0.0050000001;
c = 10;
K = bb*c/10;


A = double(subs(A));
B = double(subs(B));


%Transfer matrix
sys = ss(A, B, C, D);

t = 0:1:100;

%Input
u = ones(size(t,2),1);

[y,t,x] = lsim(sys, u, t, x0);

 

%y1 = subs(y, bb,bb1);
%y1 = subs(y1, bf,bf1);
%y1 = subs(y1, c,c1);

%y1 = simplify(y1);

plot(t,y(:,2), t, y(:,4));
%plot(t,y(:,2));

