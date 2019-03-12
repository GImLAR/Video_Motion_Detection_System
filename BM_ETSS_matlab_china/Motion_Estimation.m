%!!!The function  use the function "BME_FS/BME_E3SS/BME_DS" for motion estimation of every macroblock 
%BlockSize=[16,16]; 
%rangs=[-32,-32]; 
%range=[32,32]; 
%K=1; 
%Motion_Estimation('caltrain000.ras','caltrain010.ras',[16,16],[-32,-32],[32,32],1)
function [px,py,Predict_Img]=Motion_Estimation(TargetName,AnchorName,BlockSize,rangs,range,K) 
 
%Motion_Estimation calculate Motion Vectors in Anchor Image from Target Image in either integer or half-pel accuracy 
 
%	TargetName,AnchorName:  
%		File Names of Target Image and Anchor Image 
%  BlockSize:              
%		The size of Macro Block in Frame is BlockSize(1) by BlockSize(2) 
%	rangs,range:       
%		The Search Field in Frame A is from (rangs(1),rangs(2)) to (range(1),range(2)) 
%	K: 
%		The search accuracy: 1 integer pel 2 half pel 
%	Predict_Img: 
%		The best estimation block for the given block 
%   px,py 
%       The direction of Motion vector is (px,py)  
%	ox,oy, 
%		The location of Motion vector is (ox,oy) 
%	PSNR 
%		The peak signal and noise ratio between original image and predicted image 
%	Author: huangmin,04/2005 
 
%%%%%%%%%% for test %%%%%%%%%%%%%%%%%% 
%Target_Name=double(imread('C:\\Users\\Stankovic\\Desktop\\kinezi\\caltrain001.ras')); 
%Anchor_Name=double(imread('C:\\Users\\Stankovic\\Desktop\\kinezi\\caltrain010.ras')); 
 
%BlockSize=[16,16]; 
%rangs=[-32,-32]; 
%range=[32,32]; 
%K=1; 
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
 
Target_Img=imread(TargetName); 
Anchor_Img=imread(AnchorName); 
 
Target_Img=double(Target_Img); 
Anchor_Img=double(Anchor_Img); 
 
 
[Img_Height,Img_Width]=size(Target_Img); 
 
 
 
t=0; 
t1=cputime; 
Predict_Img=Target_Img; 
 
%Upsample the Target_Img for half-pel search 
if K==2 
  Up_Target_Img=zeros(Img_Height*2,Img_Width*2); 
	Up_Target_Img(1:2:Img_Height*2,1:2:Img_Width*2)=Target_Img; 
	Up_Target_Img(1:2:Img_Height*2-1,2:2:Img_Width*2-1)=(Target_Img(:,1:Img_Width-1)+Target_Img(:,2:Img_Width))/2; 
	Up_Target_Img(2:2:Img_Height*2-1,1:2:Img_Width*2-1)=(Target_Img(1:Img_Height-1,:)+Target_Img(2:Img_Height,:))/2; 
    Up_Target_Img(2:2:Img_Height*2-1,2:2:Img_Width*2-1)=(Target_Img(1:Img_Height-1,1:Img_Width-1)+Target_Img(1:Img_Height-1,2:Img_Width)+Target_Img(2:Img_Height,1:Img_Width-1)+Target_Img(2:Img_Height,2:Img_Width))/4; 
	Target_Imgbak=Target_Img; 
   Target_Img=Up_Target_Img; 
   t=1; 
end 
 
 
m=0; 
%Search for all the blocks in Anchor Images.  
for i=1:BlockSize(1):Img_Height-BlockSize(1)+1 
    m=m+1; 
    n=0; 
   %Caculate the search range in Target Images. 
   RangeStart(1)=i*K-t+rangs(1)*K; 
   RangeEnd(1)=i*K-t+BlockSize(1)*K-1+range(1)*K; 
   if RangeStart(1)<1 
      RangeStart(1)=1; 
   end    
   if RangeEnd(1)>Img_Height*K 
      RangeEnd(1)=Img_Height*K; 
   end 
    
   for j=1:BlockSize(2):Img_Width-BlockSize(2)+1 
       n=n+1; 
     
      RangeStart(2)=j*K-t+rangs(2)*K; 
      RangeEnd(2)=j*K-t+BlockSize(2)*K-1+range(2)*K; 
	   if RangeStart(2)<1 
   	   RangeStart(2)=1; 
	   end    
   	if RangeEnd(2)>Img_Width*K 
      	RangeEnd(2)=Img_Width*K; 
	   end 
      %Get the best estimation from Target Image. 
      [px(m,n), py(m,n), Predict_Img(i:i+BlockSize(1)-1,j:j+BlockSize(2)-1)]=BME_E3SS(Target_Img,Anchor_Img,BlockSize,[i,j],RangeStart,RangeEnd,K);  
    
      ox(m,n)=i+BlockSize(1)/2; 
      oy(m,n)=j+BlockSize(2)/2; 
     
  end 
end 
 
%Caculate the time needed 
time=cputime-t1; 
 
%Caculate the error image 
Error_Img=Anchor_Img-Predict_Img; 
 
%Caculate PSNR 
PSNR=10*log10(255*255/mean(mean((Error_Img.^2)))); 
 
 
 
 
%Display the results 
figure; 
imshow(uint8(Anchor_Img)); 
title('Anchor_Img'); 
 
 
hold on 
quiver(oy,ox,py,px); 
 
hold off 
axis image 
 
figure; 
imshow(uint8(Predict_Img)); 
title('Predict_Img') 
 
figure; 
my_plot_MV_function(Img_Height,Img_Width,BlockSize(1),px,py);set(gca,'XTick',[],'YTick',[]); 
 
 

