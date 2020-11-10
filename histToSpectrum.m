function [fr,qppm]=histToSpectrum(timeHist)
    %timeHist=importdata(filename);
    acc=timeHist';
    dt=0.01;
    intxxx=256;
    freqmin=0.1;
    freqmax=50;
    nAcc=length(timeHist);
    damp=0.02;
    frac=1.0/(intxxx-1);
    fr=zeros(1,intxxx);

    %---------------------------------------------
    a=zeros(2,2);
    b=zeros(2,2);
    dfreq=freqmax-freqmin;
    q=zeros(nAcc,3);
    qp=zeros(nAcc,3);
    qpp=zeros(nAcc,3);
    qm=zeros(intxxx,3);
    qpm=zeros(intxxx,3);
    qppm=zeros(intxxx,3);
    %------------------------------------------------
    for ifreq=1:intxxx
        freqin=freqmin+frac*dfreq*(ifreq-1);
        fr(ifreq)=freqin;
        vxsi=damp;
        freq=freqin*2.0*pi;
        den=sqrt(1.0-vxsi^2);
        vratio=1.0/den;
        freqe=den*freq;
        xsiwt=vxsi*freq*dt;
        etau=exp(-xsiwt);
        sinwt=sin(freqe*dt);
        coswt=cos(freqe*dt);
        a(1,1)=etau*(vxsi*vratio*sinwt+coswt);
        a(1,2)=etau*sinwt/freqe;
        a(2,1)=-etau*freq*vratio*sinwt;
        a(2,2)=etau*(coswt-vxsi*vratio*sinwt);

        txsi=(2.0*vxsi^2-1.0)/freq^2/freqe/dt;
        xsif=vxsi/freq/freqe;
        freqi=1.0/freq^2;

        b(1,1)=etau*(-(xsif+txsi)*sinwt-(freqi+2.0*vxsi*freqi/freq/dt)*coswt)+2.0*vxsi*freqi/freq/dt;
        b(1,2)=etau*(txsi*sinwt+2.0*vxsi*freqi/freq/dt*coswt)+freqi-2.0*vxsi*freqi/freq/dt;
        b(2,1)=etau*(-(freqe*coswt-vxsi*freq*sinwt)*(txsi+xsif)+(freqe*sinwt+vxsi*freq*coswt)*(freqi+2.0*vxsi*freqi/freq/dt))-freqi/dt;
        b(2,2)=etau*((freqe*coswt-vxsi*freq*sinwt)*txsi-(freqe*sinwt+vxsi*freq*coswt)*2.0*vxsi*freqi/freq/dt)+freqi/dt;

        for direct=1:1
            t=0;
            for it=2:nAcc
                t=t+dt;
                aq=q(it-1,direct);
                aqp=qp(it-1,direct);
                aacc=acc(it-1,direct);
                q(it,direct)=a(1,1)*aq+a(1,2)*aqp+b(1,1)*aacc+b(1,2)*acc(it,direct);
                qp(it,direct)=a(2,1)*aq+a(2,2)*aqp+b(2,1)*aacc+b(2,2)*acc(it,direct);
                qpp(it,direct)=-qp(it)*2.0*vxsi*freq-q(it,direct)*freq^2;
            end
            qmax=0;
            qpmax=0;
            qppmax=0;
            for ii=1:nAcc
                qabs=abs(q(ii,direct));
                qpabs=abs(qp(ii,direct));
                qppabs=abs(qpp(ii,direct));
                qmax=max(qmax,qabs);
                qpmax=max(qpmax,qpabs);
                qppmax=max(qppmax,qppabs);
            end
            qm(ifreq,direct)=qmax;
            qpm(ifreq,direct)=qpmax;
            qppm(ifreq,direct)=qppmax;
        end
    end
end