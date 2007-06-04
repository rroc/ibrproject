function a=imagerotation()
clc
close all
image=double(ReadPFM('church_cubemap_32.pfm'));
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




