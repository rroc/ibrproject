function testwave()

clc

image=[9 7 3 5]
mult = image.*image

de = decomposition( image )
%de = sort(de,2,'descend')
re = reconstruction( de )


de_mult = decomposition( mult )
re_mult = reconstruction( de_mult )

%ratio = de_mult./de

image2=[36 2 0.7071 -0.7071]
re_ = reconstruction( image2 )
