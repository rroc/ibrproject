function c = test_decomp()

c=[40 40 40 39 ;40 29 40 9 ;140 140 40 9 ;139 139 4 4];

g = size(c,2);
c = c/g;

while g >=2  
    %see all the rows
    for row=1:g
        c(row, 1:g)=test_decompstep(c(row, 1:g));
    end

    for col=1:g
         temp=c(1:g, col);
         c(1:g,col)= test_decompstep( temp(:,:)' )';
     end
       
    g = g/2;
end
  