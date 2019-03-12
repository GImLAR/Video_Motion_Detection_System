%plot motion field  
 
function []=my_plot_MV_function(dx,dy,bsize,mvx,mvy); 
[r,c]=size(mvy); 
[x,y]=meshgrid(0:bsize:(r-1)*bsize,0:bsize:(c-1)*bsize); 
x=x+bsize/2; y=y+bsize/2; 
mvx=mvx'; 
mvy=mvy'; 
quiver(y,x,mvy,mvx); 
axis ij 
axis([0,dy-1,0,dx-1])

