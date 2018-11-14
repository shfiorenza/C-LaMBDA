clear all;

conc = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,...
    1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,...
    2.2,2.4,2.6,2.8,3.0,3.2];

delta = [0.0,0.0,0.0,0.0,0.2,2.0,2.3,3.1,3.6,3.0,...
    3.1,3.6,3.7,3.6,3.7,3.5,3.4,3.5,3.6,3.7,...
    3.6,3.6,3.5,3.5,3.5,3.5];
vel = [0.0,0.0,0.0,0.0,120,155,160,85,120,75,50,90,60,70,50,75,...
       60,45,55,30,40,50,40,50,50,40];


delta = smooth(delta);
vel = smooth(vel);

%{
plot(conc, delta, 'LineWidth', 2);
title('Microtubule sliding stability for 4 micron-long MTs');
xlabel('Crosslinker concentration (nM)');
ylabel('Distance sliding remained stable (microns)');
%}

plot(conc, vel, 'LineWidth', 2);
title('Peak sliding velocity for 4-micron long MTs');
xlabel('Crosslinker concentration (nM)');
ylabel('Peak velocity during sliding (nm/s)');