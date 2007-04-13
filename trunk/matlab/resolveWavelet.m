function r=multWavelet( decomposed1, decomposed2 )

dsize  = size(decomposed1,2);
dsize2 = size(decomposed2,2);

if( dsize ~= dsize2 ) 
    disp('Size has to be equal'); 
    return; 
end;
    
levels = log2(dsize);

%calculate all the new elements
r(1:dsize)=0;

index=1;

for x=0: (1/dsize) :(1-(1/dsize))
    %first is the scaling coefficient
    dIndex=1;
    sum  = decomposed1(dIndex);
    sum2 = decomposed2(dIndex);
        
    %rest are detail coefficients, 
    % for each level
    for j=0:(levels-1)
        
        %for each index in the level
        for(k=1:(2^j) )
            %take the next detail coefficient,
            % AND multiply it with the corresponding wavelet
            dIndex = dIndex+1;
            d = (decomposed1(dIndex));
            d2= (decomposed2(dIndex));
            
            wfvalue = wf(j,k-1,x);
            sum = sum   + d * wfvalue;
            sum2 = sum2 + d2 * wfvalue;            
        end;
    end;

    %store the result
    r(index) = sum * sum2;
    index = index+1;
end;
