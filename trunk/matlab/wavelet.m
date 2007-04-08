function wavelet()
clc
close all
%array=[9 7 3 5];
%image=[40 40 40 39 39 39 41 41;40 29 40 9 39 239 41 141;140 140 40 9 139 139 4 4]; %imread('maki0033gs.png');
%image=double(imread('maki0033.png'))/255;
image=[   16    3   24    4    3   15    5   21;
    2    0   23   20    4    0   17   13;
    2   22   14    9   11    1    8   10;
    7    1   18   12   25   19    6   26];
vector=[1;2;3;4;5;6;7;8];
result1=image*vector
decomp = decomposition( image )
vector = decomposition(vector')
%figure
%imshow(nonstandardreconstruction( decomp ));

%decomp=decomp*2;
result2=decomp*vector'
disp('size of result');
size(result2)

result2 = (reconstruction( result2'))
%figure
%imshow(recomp);
% image2=double(imread('maki0033_ch.png'))/255;
% decomp2 = nonstandaddecomposition( image2 );
% recomp2 = nonstandardreconstruction( decomp2 );
% 
% 
% decomp3 = (decomp .* decomp2);
% decomp3 = nonstandardreconstruction( decomp3 );
% 
% image3 = image.*image2;

% figure
% imshow(image);
% figure
% imshow(decomp*300);
% figure
% imshow(recomp);
% figure
% imshow(image2);
% figure
% imshow(decomp2*300);
% figure
% imshow(recomp2);

% figure
% imshow(decomp3);
% title('Decomp');
% 
% figure
% imshow(image3);
% title('Image3');