%function [mv_x, mv_y,predict_img]=BME_E3SS(Target_Img,Anchor_Img,BlockSize,BlockLocation,RangeStart,RangeEnd,accuracy) 
function [mv_x,mv_y,Predict_Img]=BME_E3SS(Target_Img,Anchor_Img,BlockSize,BlockLocation,RangeStart,RangeEnd,accuracy) 
%[px(m,n), py(m,n), Predict_Img(i:i+BlockSize(1)-1,j:j+BlockSize(2)-1)]=BME_DS(Target_Img,Anchor_Img,BlockSize,[i,j],RangeStart,RangeEnd,K); 
%BME_E3SS compute Motion Vector for one block in Anchor Frame from Target Frame 
%use An efficient three-step search algorithm for block motion estimation 
% 
%	Target_Img,Anchor_Img:  
%		Image Matrixs of Target Frame and Anchor Frame 
%	BlockSize:              
%		The size of Macro Block is BlockSize(1) by BlockSize(2) 
%	BlcokLocation: 
%		The Macro Block Location in Anchor Frame  
%	RangeStart,RangeEnd:       
%		The Search Field in Frame A is from (RangeStart(1),RangeStart(2)) to (RangeEnd(1),RangeEnd(2)) 
%  Target_Img,Anchor_Img,Predict_Img: 
%		Image Matrix for Target Frame, Anchor Frame, Predicted Frame 
%	accuracy_x,accuracy_y: 
%		Calculation Accuracy: 1 for integer pel; 2 for half pel 
%	mv_x,mv_y: 
%		The direction of Motion vector is (mv_x,mv_y) 
%	Predict_Img: 
%		The best estimation block for given block 
%	Author: huangmin,04/2005 
 
%%%%%%%%%%%%%%%%% for test %%%%%%%%%%%%%%%%%%5 
%Target=imread('ed008.ras'); 
%Anchor=imread('ed009.ras'); 
%Target_Img=double(Target); 
%Anchor_Img=double(Anchor); 
 
%BlockSize=[64,64]; 
%BlockLocation=[96,96]; 
%RangeStart=[64,64]; 
%RangeEnd=[128,128]; 
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
 
tx=BlockLocation(1); 
ty=BlockLocation(2); 
Nx=BlockSize(1); 
Ny=BlockSize(2); 
%Get the anchor macro block 
AnchorBlock=Anchor_Img(tx:tx+Nx-1,ty:ty+Ny-1); 
 
%Initial Predict_Img 
Predict_Img=Target_Img(tx:tx+Nx-1,ty:ty+Ny-1); 
 
%Initial mv_x mv_y 
mv_x=255; 
mv_y=255; 
 
%Set caculation accuracy  
   accuracy_x=accuracy;  
   accuracy_y=accuracy; 
 
 
%Initial error 
error=255*Nx*Ny*100; 
 
 
%%%%%%%%%  four search patterns used in E3SS %%%%%%%%%%%%%%%%%%%%% 
    LDSP=[-4,-4,-4,0,0,4,4,4,-1,0,0,0,1;-4,0,4,-4,4,-4,0,4,0,-1,0,1,0]; 
    SDSP=[-1,0,0,0,1;0,-1,0,1,0];  
    SSP1=[-2,-2,-2,0,0,0,2,2,2;-2,0,2,-2,0,2,-2,0,2]; 
    SSP2=[-1,-1,-1,0,0,0,1,1,1;-1,0,1,-1,0,1,-1,0,1]; 
     
    nx=tx;%tx=BlockLocation(1); 
    ny=ty;%ty=BlockLocation(2); 
     
  
    for j=1:1:13 
         
            %LDSP=[-4,-4,-4,0,0,4,4,4,-1,0,0,0,1;-4,0,4,-4,4,-4,0,4,0,-1,0,1,0]; 
            nxS=nx+LDSP(1,j);      %nx-4??nx+4??nx-1??nx+1 
            nxE=nx+LDSP(1,j)+Nx-1; %Nx=BlockSize(1) 
            nyS=ny+LDSP(2,j);      %ny-4??ny+4??ny-1??ny+1 
            nyE=ny+LDSP(2,j)+Ny-1; %Ny=BlockSize(2) 
              
    %Search the best estimation from (RangeStart(1),RangeStart(2)) to (RangeEnd(1),RangeEnd(2))       
        if (nxS>=RangeStart(1))&(nxE<=RangeEnd(1))&(nyS>=RangeStart(2))&(nyE<=RangeEnd(2)) 
             
          %reference image  :Target_Img        
          down_Target_Img0=Target_Img(nxS:accuracy_x:nxE,nyS:accuracy_y:nyE);          
           
           %caculate the error 
           temp_error=sum(sum(abs(AnchorBlock-down_Target_Img0))); 
		   if temp_error < error 
			 error=temp_error; 
			 mv_x=LDSP(1,j); 
             mv_y=LDSP(2,j); 
             Predict_Img=down_Target_Img0; 
           end 
        end 
    end 
        
    %%%%%%%%% change the center point %%%%%%%%%% 
       nx=nx+mv_x;        
       ny=ny+mv_y; 
      
  
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
   % if BDM is one of the eight points in 9*9 pattern  
   % SSP1=[-2,-2,-2,0,0,0,2,2,2;-2,0,2,-2,0,2,-2,0,2]; 
 if (abs(mv_x)==4|abs(mv_y)==4) 
       %as same as 3SS   
       for j=1:1:9 
           nxS=nx+SSP1(1,j); 
            nxE=nx+SSP1(1,j)+Nx-1; 
            nyS=ny+SSP1(2,j); 
            nyE=ny+SSP1(2,j)+Ny-1; 
             
        if (nxS>=RangeStart(1))&(nxE<=RangeEnd(1))&(nyS>=RangeStart(2))&(nyE<=RangeEnd(2))                      
           
         down_Target_Img1=Target_Img(nxS:accuracy_x:nxE,nyS:accuracy_y:nyE);        
           
           %caculate the error 
           temp_error=sum(sum(abs(AnchorBlock-down_Target_Img1))); 
		  if temp_error < error 
			 error=temp_error; 
			 mv_x=SSP1(1,j); 
             mv_y=SSP1(2,j); 
              Predict_Img=down_Target_Img1; 
          end 
      end; 
  end 
      
    %%%%%%%%% change the center point %%%%%%%%%% 
     nx=nx+mv_x;        
       ny=ny+mv_y; 
 
      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
     %SSP2=[-1,-1,-1,0,0,0,1,1,1;-1,0,1,-1,0,1,-1,0,1]; 
     for j=1:1:9 
             
           nxS=nx+SSP2(1,j); 
            nxE=nx+SSP2(1,j)+Nx-1; 
            nyS=ny+SSP2(2,j); 
            nyE=ny+SSP2(2,j)+Ny-1; 
             
          if (nxS>=RangeStart(1))&(nxE<=RangeEnd(1))&(nyS>=RangeStart(2))&(nyE<=RangeEnd(2))           
                  down_Target_Img2=Target_Img(nxS:accuracy_x:nxE,nyS:accuracy_y:nyE);        
            
           %caculate the error 
           temp_error=sum(sum(abs(AnchorBlock-down_Target_Img2))); 
		  if temp_error < error 
			 error=temp_error; 
			 mv_x=SSP2(1,j); 
             mv_y=SSP2(2,j); 
              Predict_Img=down_Target_Img2; 
           end 
      end; 
  end 
       
       %%%%%%%%% change the center point %%%%%%%%%% 
        nx=nx+mv_x;        
        ny=ny+mv_y; 
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   
     % if BDM is not one of the eight points in 9*9 pattern 
     % SDSP=[-1,0,0,0,1;0,-1,0,1,0];  
     else  
       while ((mv_x~=0|mv_y~=0)&((nx-2)>=RangeStart(1))&((nx+2)<=RangeEnd(1))&((ny-2)<RangeStart(2))&((ny+2)>RangeEnd(2))) 
         for j=1:1:5 
              
            nxS=nx+SDSP(1,j); 
            nxE=nx+SDSP(1,j)+Nx-1; 
            nyS=ny+SDSP(2,j); 
            nyE=ny+SDSP(2,j)+Ny-1; 
             
            if (nxS>=RangeStart(1))&(nxE<=RangeEnd(1))&(nyS>=RangeStart(2))&(nyE<=RangeEnd(2)) 
             down_Target_Img3=Target_Img(nxS:accuracy_x:nxE,nyS:accuracy_y:nyE);               
                    
            %caculate the error 
              temp_error=sum(sum(abs(AnchorBlock-down_Target_Img3))); 
		      if temp_error < error 
			     error=temp_error; 
			     mv_x=SDSP(1,j); 
                 mv_y=SDSP(2,j);   
                Predict_Img=down_Target_Img3; 
               end 
           end 
      end 
       
        %%%%%%%%% change the center point %%%%%%%%%% 
          nx=nx+mv_x; 
          ny=ny+mv_y; 
        end  
        
     end 
      
     mv_x=nx-tx; 
     mv_y=ny-ty; 
      
%%%%%%%%%%%%%%%%%%%%%%%%5 for test %%%%%%%%%%%%%%%%%%%%%%% 
%figure 
%imshow(uint8(AnchorBlock)); 
%title('AnchorBlock'); 
 
%figure 
%imshow(uint8(predict_img)); 
%title('predict_img'); 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


