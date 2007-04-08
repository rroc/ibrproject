clc;
n = 10; 
lev = 8; 
wav = 'haar';

% Wavelet based matrix multiplication by a vector: 
% a "good" example 
% Matrix is magic(512) Vector is (1:512)

m=[   16    3   24    4    3   15    5   21;
    2    0   23   20    4    0   17   13;
    2   22   14    9   11    1    8   10;
    7    1   18   12   25   19    6   26];
v=[1;2;3;4;5;6;7;8];

%m = magic(n)
%v = (1:n)'

% ordinary matrix multiplication by a vector. 
p = m * v


[Lo_D,Hi_D,Lo_R,Hi_R] = wfilters(wav);

% Compute matrix approximation at level 5. 
sm = m;
for i = 1:lev 
    sm = dyaddown(conv2(sm,Lo_D),'c'); 
    sm = dyaddown(conv2(sm,Lo_D'),'r'); 
end 

sm
sm2 = decomposition( m )

% The three steps: 
% 1. Compute vector approximation. 
% 2. Compute multiplication in wavelet domain. 
% 3. Reconstruct vector approximation.
sv = v; 
for i = 1:lev, sv = dyaddown(conv(sv,Lo_D)); end 

sv
sv2 = decomposition(v')


%multiply transformed
sp = sm * sv;

for i = 1:lev, sp = conv(dyadup(sp),Lo_R); end 
sp = wkeep(sp,length(v))

%sp = sp';

% Relative square norm error in percent when using wavelets. 
%rnrm = 100 * (norm(p-sp)/norm(p))
