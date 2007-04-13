function wavelet()

close all
%array=[9 7 3 5];
%image=[40 40 40 39 39 39 41 41;40 29 40 9 39 239 41 141;140 140 40 9 139 139 4 4]; %imread('maki0033gs.png');
%image=double(imread('church_roof.png'));
%image=double(ReadPFM('4_4_test.pfm'))
image=double(ReadPFM('church_mirror.pfm'));
imshow(image);
title('Original');

decomp = nonstandarddecomposition( image );
figure
imshow(decomp);
title('Original -> decomposed');

re_mul = nonstandardreconstruction( decomp );
figure
imshow(re_mul);
title('decomposed -> back_t_o original');

im_mul = image.*image;
figure
imshow(im_mul);
title('original * original');

de_mul = decomp.*decomp;
figure
imshow(de_mul);
title('decomposed * decomposed');

re_mul = nonstandardreconstruction( de_mul );
figure
imshow(re_mul);
title('decomposed * decomposed -> back_t_o original');

%recomp = reconstruction( decomp );
%figure
%imshow(recomp);
