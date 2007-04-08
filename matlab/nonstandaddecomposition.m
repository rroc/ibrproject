function c = nonstandaddecomposition( c )

g = size(c,2);
c = c/g;

while g >=2  
    %see all the rows
    for row=1:g
        for color=1:3
             c(row, 1:g, color)=decompositionstep(c(row, 1:g, color));
        end
    end

    for col=1:g
         temp=c(1:g, col, :);
         for color=1:3
              c(1:g,col, color)= decompositionstep( temp(:,:,color)' )';
         end
    end
    
    %all the columns
%     for col=1:g
%         temp=c(1:g, col, :);
%         temp(:,:,1) = temp(:,:,1)';
%         temp(:,:,2) = temp(:,:,2)';
%         temp(:,:,3) = temp(:,:,3)';
%         temp = decompositionstep( temp );
%         temp(:,:,1) = temp(:,:,1)';
%         temp(:,:,2) = temp(:,:,2)';
%         temp(:,:,3) = temp(:,:,3)';
%         c(1:g,col, :) =  temp;
%     end
    
    g = g/2;
end
    