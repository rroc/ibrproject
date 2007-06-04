function imagerotation()

close all
image=double(ReadPFM('church_cubemap_32.pfm'));
imshow(image);
title('Original');

