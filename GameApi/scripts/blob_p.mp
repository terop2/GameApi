PT I1=ev.point_api.point(75,0,0);
FD I2=ev.dist_api.sphere(I1,100.0);
PT I3=ev.point_api.point(-75,0,0);
FD I4=ev.dist_api.sphere(I3,100);
FD I5=ev.dist_api.blend(I2,I4,35);
P I6=ev.dist_api.distance_poly_sph(ev,I5,400,400,400,512,512,40,40,50,50,50,50);
