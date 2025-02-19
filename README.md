# Drone-Simulator
Jocul a fost realizat in cadrul materiei de Elemente de Grafica pe Calculator
si a avut ca scop realizarea si simularea unei drone in spatiu 3D, generarea
de elemente (case, copaci) pe o harta, cat si realizarea unui mini-joc de
livrare a unor pachete.

Codul dronei se gaseste in fisierul Drone-Simulator-code, iar pentru compilare
in gfx-framework-master>src>main.cpp
Se recomanda folosirea unui mediu precum VisualStudio si e necesar cmake.

Drona simuleaza modului de zbor Angle, iar comenzile pentru acesta sunt
Rotatie in jurul axei OY: A/D
Rotatie in jurul axei OX: UP/DOWN
Rotatie in jurul axei OZ: LEFT/RIGHT
Controlul thrust: W/S
Rotatie camera: J/L

Scopul jocului este sa livrezi cat mai multe pachete, la coliziunea cu solul
sau cu obiecte, pachetul este returnat la zona de ridicare, iar pentru a
putea primi punctul, acesta trebuie sa ajunga la zona de livrare.
Pe harta se afla o sageata care indica directa pachetului, cat si un minimap.