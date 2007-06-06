function a=imagerotation()
clc
close all
image=double(ReadPFM('test_cubemap_32.pfm'));
%figure;imshow(image);title('original');

%cut cubemap
%-----------
[h,w,colors] = size(image);

facew = w/3-1;
faceh = h/4-1;
% empty = imcrop(image,[2*(w/3)+1 0 facew-2 faceh-1 ]);
empty2 = imcrop(image,[2*(w/3)+1 0 facew faceh+1 ]);
empty=empty2;

roof = imcrop(image,[w/3+1 0 facew faceh+1 ]);
%figure;imshow(roof);title('roof');
left = imcrop(image,[0 h/4+1 facew+1 faceh]);
%figure;imshow(left);title('left');
front = imcrop(image,[w/3+1 h/4+1 facew faceh]);
%figure;imshow(front);title('front');
right = imcrop(image,[2*(w/3)+1 h/4+1 facew faceh]);
%figure;imshow(right);title('right');
floor = imcrop(image,[w/3+1  2*(h/4)+1 facew faceh]);
%figure;imshow(floor);title('floor');
back = imcrop(image,[w/3+1 3*(h/4)+1 facew faceh]);
%figure;imshow(back);title('back');

% size(empty2)
% size(roof)
% size(left)
% size(front)
% size(right)
% size(floor)
% size(back)

loaded = [empty2 roof empty2;left front right;empty2 floor empty2; empty2 back empty2];
figure;imshow(loaded);title('loaded');

%calculate vectors for faces
index = 1;
%dx = 1.0/(facew);dy = 1.0/(faceh);
for y=1:32
    for x=1:32
        %disp(['(',num2str(y),',',num2str(x),')']);
        vec_roof( index,:) = CubeToVector(1, x/32.0, y/32.0);
        vec_left( index,:) = CubeToVector(2, x/32.0, y/32.0);
        vec_front( index,:)= CubeToVector(3, x/32.0, y/32.0);
        vec_right( index,:)= CubeToVector(4, x/32.0, y/32.0);
        vec_floor( index,:)= CubeToVector(5, x/32.0, y/32.0);
        vec_back( index,:) = CubeToVector(6, x/32.0, y/32.0);

        index = index+1;
    end
end


%APPLY ROTATION
angle = -(pi/180)*90;
uv_roof = rotateVectors(angle,0,0, vec_roof);
uv_left = rotateVectors(angle,0,0, vec_left);
uv_front= rotateVectors(angle,0,0, vec_front);
uv_right= rotateVectors(angle,0,0, vec_right);
uv_floor= rotateVectors(angle,0,0, vec_floor);
uv_back = rotateVectors(angle,0,0, vec_back);


imroof = createImage( uv_roof, ... 
            uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
            roof, left, front, right, floor, back );
%figure;imshow(imroof);title('rotated');
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

% size(empty)
% size(imroof)
% size(imleft)
% size(imfront)
% size(imright)
% size(imfloor)
% size(imback)

output = [empty imroof empty;imleft imfront imright;empty imfloor empty; empty imback empty];
figure;imshow(output);title('rotated');