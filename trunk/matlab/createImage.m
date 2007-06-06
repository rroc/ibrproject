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
        
        %roof
        if(faceid == 1)
            u = (round( uv_roof(index,2) *length))+1;
            v = (round( uv_roof(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = roof( u, v, : );
        elseif(faceid == 2)
            u = (round( uv_left(index,2) *length))+1;
            v = (round( uv_left(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = left( u, v, : );
        elseif(faceid == 3)
            u = (round( uv_front(index,2) *length))+1;
            v = (round( uv_front(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = front( u, v, : );
        elseif(faceid == 4)
            u = (round( uv_right(index,2) *length))+1;
            v = (round( uv_right(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = right( u, v, : );
        elseif(faceid == 5)
            u = (round( uv_floor(index,2) *length))+1;
            v = (round( uv_floor(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = floor( u, v, : );
        elseif(faceid == 6)
            u = (round( uv_back(index,2) *length))+1;
            v = (round( uv_back(index,3) *length))+1;
            
            if(u>31) u=31; end
            if(v>31) v=31; end
            imageout(x,y,:) = back( u, v, : );
        else
            imageout(x,y,:) = [1/faceid 1/faceid 1/faceid];
            %faceid;
        end
    end
end
