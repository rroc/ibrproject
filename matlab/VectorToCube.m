function vector=VectorToCube(aV)

%front
if( aV(3)>=0 && aV(3)>=abs(aV(1)) && aV(3)>= abs(aV(2)) )
    vector=[3 0.5-0.5*aV(2)/aV(3) 0.5+0.5*aV(1)/aV(3) ];
%back
elseif( aV(3)<0 && aV(3)<=-abs(aV(1)) && aV(3)<= -abs(aV(2)) )
    vector=[6 0.5-0.5*aV(2)/aV(3) 0.5-0.5*aV(1)/aV(3) ];
%floor
elseif( aV(2)<=0 && aV(2)<=-abs(aV(1)) && aV(2)<= -abs(aV(3)) )
    vector=[5 0.5+0.5*aV(3)/aV(2) 0.5-0.5*aV(1)/aV(2) ];
%roof
elseif( aV(2)>=0 && aV(2)>=abs(aV(1)) && aV(2)>= abs(aV(3)) )
    vector=[1 0.5+0.5*aV(3)/aV(2) 0.5+0.5*aV(1)/aV(2) ];
%left
elseif( aV(1)<=0 && aV(1)<=-abs(aV(2)) && aV(1)<= -abs(aV(3)) )
    vector=[2 0.5+0.5*aV(2)/aV(1) 0.5-0.5*aV(3)/aV(1) ];
%right
elseif( aV(1)>=0 && aV(1)>=abs(aV(2)) && aV(1)>= abs(aV(3)) )
    vector=[4 0.5-0.5*aV(2)/aV(1) 0.5-0.5*aV(3)/aV(1) ];
else
    vector=[0 0 0];
end



% %front
% if( aV(3)<0 && aV(3)<=-abs(aV(1)) && aV(3)<= -abs(aV(2)) )
%     vector=[6 0.5-0.5*aV(2)/aV(3) 0.5-0.5*aV(1)/aV(3)];
% %back
% elseif( aV(3)>=0 && aV(3)>=abs(aV(1)) && aV(3)>= abs(aV(2)) )
%     vector=[3 0.5-0.5*aV(2)/aV(3) 0.5+0.5*aV(1)/aV(3)];
% %left
% elseif( aV(1)<=0 && aV(1)<=-abs(aV(2)) && aV(1)<= -abs(aV(3)) )
%     vector=[2 0.5+0.5*aV(2)/aV(1) 0.5-0.5*aV(3)/aV(1)];
% %right
% elseif( aV(1)>=0 && aV(1)>=abs(aV(2)) && aV(1)>= abs(aV(3)) )
%     vector=[4 0.5-0.5*aV(2)/aV(1) 0.5-0.5*aV(3)/aV(1)];    
% %floor
% elseif( aV(2)<=0 && aV(2)<=-abs(aV(1)) && aV(2)<= -abs(aV(3)) )
%     vector=[5 0.5+0.5*aV(3)/aV(2) 0.5-0.5*aV(1)/aV(2)];
% %roof
% elseif( aV(2)>=0 && aV(2)>=abs(aV(1)) && aV(2)>= abs(aV(3)) )
%     vector=[1 0.5+0.5*aV(3)/aV(2) 0.5+0.5*aV(1)/aV(2)];
% end
