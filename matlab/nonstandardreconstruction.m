function c=nonstandardreconstruction(c)
g=2;
while g<=size(c,2)
    for col=1:g
        temp=c(1:g, col, :);
        for color=1:3
            c(1:g,col,color)=reconstructionstep( temp(:,:,color)' )';
        end
    end
     
    for row=1:g
        for color=1:3
            c(row,1:g,color)=reconstructionstep(c(row,1:g,color));
        end
    end
    g=2*g;
end
c=c*size(c,2);
