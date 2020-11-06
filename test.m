clear all;
clc;
dt=0.01;
T=13.75;
deltaFreq=0.02;
omegac=2*pi*deltaFreq;
freq=0.1;
omega=2*pi*freq;
t=[0:dt:40.95];
for i=1:length(t)
    a(i)=1*(sin(omegac*(t(i)-T))/omegac/(t(i)-T))^5*cos(omega*(t(i)-T));
end
plot(t,a);