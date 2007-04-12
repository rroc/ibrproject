function c=decomposition(c)

%format long
%normalize all
columns=size(c,2);
c=c/sqrt(columns);


%pass sections of the imacolumnse to the decompositestep function
%each section should be half of the size of the previous

rows=size(c,1);

for row=1:rows
    columns=size(c,2);

    while columns>=2 
    %    disp(['Step: ', num2str(columns) ]);
        c(row, 1:columns, :)=decompositionstep(c(row, 1:columns, :));
        columns=columns/2;
    end
end


    
