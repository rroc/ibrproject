function imageout = createImage( uv, ...
                uv_roof, uv_left, uv_front, uv_right, uv_floor, uv_back, ...
                roof, left, front, right, floor, back )

%create image
%disp(['uv: ', num2str( size( uv,1)) ]);
length = sqrt( size( uv,1 ) );


for y=1:length
    for x=1:length  
        index = (x) + (y-1)*length;
                    
        faceid = uv(index,1);
        u = (ceil( uv(index,2)*length))+1;
        v = (ceil( uv(index,3)*length))+1;
            
%         if( u~=1 && v~=1)
%         disp(['vec: ', num2str([u v])]);
%         end;

        %roof
        if(faceid == 1)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = roof( u, v, : );
        elseif(faceid == 2)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = left( u, v, : );
        elseif(faceid == 3)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = front( u, v, : );
        elseif(faceid == 4)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = right( u, v, : );
        elseif(faceid == 5)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = floor( u, v, : );
        elseif(faceid == 6)
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = back( u, v, : );
        else
            imageout(x,y,:) = [1 1 1];%[1/faceid 1/faceid 1/faceid];
            %faceid;
        end
    end
end
