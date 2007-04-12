function r=resolveWavelet( decomposed )


dsize = size(decomposed,2);
levels = log2(dsize);
    


%calculate all the new elements
r(1:dsize)=0;

index=1;

for x=0: (1/dsize) :(1-(1/dsize))
    %first is the scaling coefficient
    dIndex=1;
    sum = decomposed(dIndex);
        
    %rest are detail coefficients, 
    % for each level
    for j=0:(levels-1)
        
        %for each index in the leve
        for(k=1:(2^j) )
            
            %take the next detail coefficient,
            % AND multiply it with the corresponding wavelet
            dIndex = dIndex+1;
            d = (decomposed(dIndex));
            wfvalue = wf(j,k-1,x);
            sum = sum + d * d * wfvalue;            
        end;
    end;

    %store the result
    r(index) = sum;
    index = index+1;
end;
