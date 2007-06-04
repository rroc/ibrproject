function vector=VectorToCube(aV)
if( aV(3)<0 && aV(3)<=-abs(aV(1)) && aV(3)<= -abs(aV(2)) )
    vector=[3 0.5-0.5*aV(1)/aV(3) 0.5+0.5*aV(2)/aV(3) ];
elseif( aV(3)>=0 && aV(3)>=abs(aV(1)) && aV(3)>= abs(aV(2)) )
    vector=[6 0.5+0.5*aV(1)/aV(3) 0.5+0.5*aV(2)/aV(3) ];
elseif( aV(2)<=0 && aV(2)<=-abs(aV(1)) && aV(2)<= -abs(aV(3)) )
    vector=[5 0.5-0.5*aV(1)/aV(2) 0.5-0.5*aV(3)/aV(2) ];
elseif( aV(2)>=0 && aV(2)>=abs(aV(1)) && aV(2)>= abs(aV(3)) )
    vector=[1 0.5+0.5*aV(1)/aV(2) 0.5-0.5*aV(3)/aV(2) ];
elseif( aV(1)<=0 && aV(1)<=-abs(aV(2)) && aV(1)<= -abs(aV(3)) )
    vector=[2 0.5+0.5*aV(3)/aV(1) 0.5+0.5*aV(2)/aV(1) ];
elseif( aV(1)>=0 && aV(1)>=abs(aV(2)) && aV(1)>= abs(aV(3)) )
    vector=[4 0.5+0.5*aV(3)/aV(1) 0.5-0.5*aV(2)/aV(1) ];
end
