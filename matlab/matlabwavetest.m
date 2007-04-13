% The current extension mode is zero-padding (see dwtmode).

% Load original one-dimensional signal. 
%load leleccum; s = leleccum(1:3920); ls = length(s); 

s=[9 7 3 5];

% Perform decomposition of signal at level 3 using db5. 
[decomposed,l] = wavedec(s,2,'haar')

% Reconstruct s from the wavelet decomposition structure [c,l]. 
reconstructed = waverec(decomposed,l,'haar')

