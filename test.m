clear all;
clc;
dt=0.01;
T=18.94;
omegac=0.0125664*10;
% omegac=0.5*pi;
deltaS=0.0750719;
% omega=20*2*pi;
omega=0.628319;
t=[0:dt:40.95];
nSize=length(t);
factor=(sin(omegac*(0-T))/omegac/(0-T))^4
for i=1:length(t)
    if(abs(t(i)-T)<1.0e-4)
        a(i)=deltaS;
        continue;
    end
    a(i)=deltaS*(sin(omegac*(t(i)-T))/omegac/(t(i)-T))^2*cos(omega*(t(i)-T));
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
for i=2:length(t)/2
    Aww(i)=Aw(i)/Hw(i);
    Aww(length(t)+2-i)=conj(Aww(i));
end
Aww(nSize/2+1)=Aw(nSize/2+1)/abs(Hw(nSize/2+1));
b=ifft(Aww);
% plot(t,a)
plot(t,real(b),t,abs(a));