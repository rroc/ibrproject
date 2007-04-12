function testwave()

%clc

%image=[9 7 3 5];
image=[9 7 3 5 9 7 3 5 9 7 3 5 9 7 3 5 9 7 3 5 9 7 3 5 9 7 3 5 9 7 3 5];
%image=[0 0 0 1];
de = decomposition( image );
re = reconstruction( de );

disp(['I = [',num2str(image),']'])
disp(['to wavelet  -> WI = [',num2str(de),']']);
disp(['reconstruct -> I_ = [',num2str(re),']']);
disp(' ');disp(' ');

mult = image.*image;
%de_mult= ((de*sqrt(size(image,2))).*(de*sqrt(size(image,2)))) / sqrt(size(image,2));
de_mult= de.*de;
re_mult = reconstruction( de_mult );

disp('Multiplication:');
disp(' ');
disp('Expected result: (in original domain)');
disp(['I.*I = [',num2str(mult),'] =>  sum = ',num2str(sum(mult))]);
disp([' ( in wavelet: W(I*I)= [',num2str( decomposition(mult) ),'] )']);
disp(' ');
disp('Our result: (in wavelet domain)');
disp(['WI.*WI = [',num2str(de_mult),']']);
disp(['reconstruct ->   I_ = [',num2str(re_mult),'] =>  sum = ',num2str(sum(re_mult))]);
disp(' ');

