#j,-259,-251.48,0,A,FIXED
#j,0,0,0,B
#j,0,-251.48,0,C
#j,259,-251.48,0,D,ROLLING
#m,0,1,AB
#m,1,3,BD
#m,1,2,BC
#m,2,3,CD
#m,0,2,AC

#Scale factor between tangible model and real model(real models are in the order of meters)(affect only the position of the joints)
0.1

#Number of nodes
4

#Node id,x,y,z,dim(always 0)
0,-2500,-2500,0,0
1,0,-50,0,0
2,0,-2500,0,0
3,2500,-2500,0,0

#Number of reaction
4

#Node Id,x,y,z,Mx,My,Mz
0,1,1,1,1,1,0
1,0,0,1,1,1,0
2,0,0,1,1,1,0
3,0,1,1,1,1,0

#Number of beams
5

#Beam name,node 1,node 2,width,height,Youngh,E,d
AB,0,1,120,220,20000,1250,0.50e-9
BD,1,3,120,220,20000,1250,0.50e-9
BC,2,1,120,220,20000,1250,0.50e-9
CD,2,3,120,220,20000,1250,0.50e-9
AC,0,2,120,220,20000,1250,0.50e-9
