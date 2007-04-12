% Calculates the wavelet function
% j = level (0->)
% k = index ( 0 < k < (2^j-1) ) (ie. the segment index in each level)
% x = 
function r = wf( j, k, x )

r = sqrt(2^j) * w( (2^j)*x - k );
