function plotsurf(r)
% Plot a sphere, r is the shpere's radius.
if nargin < 1
    r=2;
end
theta=linspace(0,pi,20);
phi=linspace(0,2*pi,40);
% Create two matrices for theta and phi
[tt, pp]=meshgrid(theta, phi);
% Get matrices for x ,y and z from theta and phi
xx=r*sin(tt).*cos(pp);
yy=r*sin(tt).*sin(pp);
zz=r*cos(tt);
% Plot Surface
surf(xx,yy,zz);
% Customize labels and titles
xlabel('x');
ylabel('y');
zlabel('z');
str1=sprintf('x^2+y^2+z^2=%.1f^2', r);
title(str1);
axis equal;
% Color shading of surface
shading flat;