function r=w(value)

if( value>=0 && value < 0.5)
    r=1;
elseif( value>=0.5 && value < 1.0)
    r=-1;
else
    r=0;
end;
