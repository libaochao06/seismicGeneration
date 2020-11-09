clear all;
clc;
dt=0.01;
T=18.94;
omegac=0.125664;
deltaS=0.0750719;
omega=0.628319;
t=[0:dt:40.95];
for i=1:length(t)
    if(abs(t(i)-T)<1.0e-4)
        a(i)=deltaS;
        continue;
    end
    a(i)=deltaS*(sin(omegac*(t(i)-T))/omegac/(t(i)-T))^4*cos(omega*(t(i)-T));
end
% plot(t,a);

damp=0.02;
deltaFreq=1/40.95;
freqs=[0:deltaFreq:deltaFreq*4095];
omega0=omega/sqrt(1-damp^2);
for i=1:length(t)
    ome(i)=deltaFreq*2*pi*(i-1);
    up=complex(omega0^2,2*damp*omega0*ome(i));
    down=complex(omega0^2-ome(i)^2,2*damp*ome(i)*omega0);
    Hw(i)=up/down;
end

Aw=fft(a);
for i=1:length(t)
    Aww(i)=Aw(i)/Hw(i);
end
b=ifft(Aww);
% plot(t,real(b),t,a);