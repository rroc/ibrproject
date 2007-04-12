function c=reconstruction(c)

format long

rows=size(c,1);

for row=1:rows
    columns=2;
    while columns<=size(c,2)
        c(row,1:columns,:)=reconstructionstep(c(row,1:columns,:));
        columns=2*columns;
    end
end

%renormalize
c=c*sqrt(size(c,2));    
