function a=imagerotation()
clc
close all
%image=double(ReadPFM('test_cubemap_32.pfm'));
%image=double(ReadPFM('test2_cubemap_32.pfm'));
image=double(ReadPFM('church_cubemap_32.pfm'));
%figure;imshow(image);title('original');

%cut cubemap
%-----------
[h,w,colors] = size(image);

facew = w/3-1;
faceh = h/4-1;
empty = imcrop(image,[2*(w/3)+1 0 facew faceh+1 ]);

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

loaded = [empty roof empty;left front right;empty floor empty; empty back empty];
figure;imshow(imresize(loaded,2.0));title('loaded');

%calculate vectors for faces
index = 1;
%dx = 1.0/(facew);dy = 1.0/(faceh);
for y=1:32
    for x=1:32
        %disp(['(',num2str(y),',',num2str(x),')']);
        vec_roof( index,:)= CubeToVector(1, x/32.0, y/32.0);
        vec_left( index,:)= CubeToVector(2, x/32.0, y/32.0);
        vec_front(index,:)= CubeToVector(3, x/32.0, y/32.0);
        vec_right(index,:)= CubeToVector(4, x/32.0, y/32.0);
        vec_floor(index,:)= CubeToVector(5, x/32.0, y/32.0);
        vec_back( index,:)= CubeToVector(6, x/32.0, y/32.0);        
        index = index+1;
    end
end

drawNormals( vec_roof,vec_left,vec_front,vec_right, vec_floor, vec_back, empty );

%dimensions
% disp(['roof : ', num2str( vec_roof (1,:) ),' <-> ', num2str( vec_roof (index-1,:) )]); 
% disp(['left : ', num2str( vec_left (1,:) ),' <-> ', num2str( vec_left (index-1,:) )]); 
% disp(['front: ', num2str( vec_front(1,:) ),' <-> ', num2str( vec_front(index-1,:) )]); 
% disp(['right: ', num2str( vec_right(1,:) ),' <-> ', num2str( vec_right(index-1,:) )]); 
% disp(['floor: ', num2str( vec_floor(1,:) ),' <-> ', num2str( vec_floor(index-1,:) )]); 
% disp(['back : ', num2str( vec_back (1,:) ),' <-> ', num2str( vec_back (index-1,:) )]); 

for i=360:10:360  
    %APPLY ROTATION
    angle = (pi/180)*i;

    [uv_roof,  roofnormals] = rotateVectors(angle/2,angle,0, vec_roof);
    [uv_left,  leftnormals] = rotateVectors(angle/2,angle,0, vec_left);
    [uv_front, frontnormals]= rotateVectors(angle/2,angle,0, vec_front);
    [uv_right, rightnormals]= rotateVectors(angle/2,angle,0, vec_right);
    [uv_floor, floornormals]= rotateVectors(angle/2,angle,0, vec_floor);
    [uv_back,  backnormals] = rotateVectors(angle/2,angle,0, vec_back);

    %drawNormals( roofnormals,leftnormals,frontnormals,rightnormals, floornormals, backnormals, empty );

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

    output = [empty imroof empty;imleft imfront imright;empty imfloor empty; empty imback empty];
    figure;imshow(imresize(output,2.0));title('rotated');
end;
