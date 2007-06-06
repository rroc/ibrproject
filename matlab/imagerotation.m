function a=imagerotation()
clc
close all
image=double(ReadPFM('test_cubemap_32.pfm'));
figure;imshow(image);title('original');

%cut cubemap
%-----------
[h,w,colors] = size(image);

facew = w/3-1;
faceh = h/4-1;
roof = imcrop(image,[w/3+1 0 facew faceh ]);
%figure;imshow(roof);title('roof');
left = imcrop(image,[0 h/4+1 facew faceh]);
%figure;imshow(left);title('left');
front = imcrop(image,[w/3+1 h/4+1 facew faceh]);
%figure;imshow(front);title('front');
right = imcrop(image,[2*(w/3)+1 h/4+1 facew faceh]);
%figure;imshow(right);title('right');
floor = imcrop(image,[w/3+1  2*(h/4)+1 facew faceh]);
%figure;imshow(floor);title('floor');
back = imcrop(image,[w/3+1 3*(h/4)+1 facew faceh]);
%figure;imshow(back);title('back');

%calculate vectors for faces
index = 1;
dx = 1.0/(facew);dy = 1.0/(faceh);
for y=dy:dy:1.0
    for x=dx:dx:1.0
        %disp(['(',num2str(y),',',num2str(x),')']);
        vec_roof( index,:) = CubeToVector(1, x, y);
        vec_left( index,:) = CubeToVector(2, x, y);
        vec_front( index,:)= CubeToVector(3, x, y);
        vec_right( index,:)= CubeToVector(4, x, y);
        vec_floor( index,:)= CubeToVector(5, x, y);
        vec_back( index,:) = CubeToVector(6, x, y);

        index = index+1;
    end
end

% %get pixel coordinates
% length = size( vec_roof,1 );
% for i=1:length
%     uv_roof(i,:) = VectorToCube( vec_roof(i, :) );
% end
% 
% %create image
% length = sqrt( size( uv_roof,1 ) );
% for y=1:length
%     for x=1:length  
%         index = x + (y-1)*length;
%             
%         %uv_roof(index,:)
%         
%         faceid = uv_roof(index,1);
%         %roof
%         if(faceid == 1)
%             u = (round( uv_roof(index,2) *length));
%             v = (round( uv_roof(index,3) *length));
%             imageout(x,y,:) = roof( u, v, : );
%         else
%             imageout(x,y,:) = [1/faceid 1/faceid 1/faceid];
%             faceid
%         end
%     end
% end
% 
% figure;imshow(imageout);title('output');


%APPLY ROTATION
angle = pi/4;
uv_roof = rotateVectors(angle, vec_roof);
uv_left = rotateVectors(angle, vec_left);
uv_front= rotateVectors(angle, vec_front);
uv_right= rotateVectors(angle, vec_right);
uv_floor= rotateVectors(angle, vec_floor);
uv_back = rotateVectors(angle, vec_back);


imroof = createImage( uv_roof, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
figure;imshow(imroof);title('rotated');
imleft = createImage( uv_left, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imleft);title('rotated');
imfront = createImage( uv_front, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imfront);title('rotated');
imright = createImage( uv_right, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imright);title('rotated');
imfloor = createImage( uv_floor, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imfloor);title('rotated');
imback = createImage( uv_back, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imback);title('rotated');

