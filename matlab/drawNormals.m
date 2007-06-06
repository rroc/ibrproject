function drawNormals( vec_roof,vec_left,vec_front,vec_right, vec_floor, vec_back, empty )

%calculate vectors for faces
index = 1;
%dx = 1.0/(facew);dy = 1.0/(faceh);
for y=1:32
    for x=1:32
        %disp(['(',num2str(y),',',num2str(x),')']);
        nroof(x,y,:)  = (1+vec_roof( index,:) ) /2;
        nleft(x,y,:)  = (1+vec_left( index,:) ) /2;
        nfront(x,y,:) = (1+vec_front( index,:)) /2;
        nright(x,y,:) = (1+vec_right( index,:)) /2;
        nfloor(x,y,:) = (1+vec_floor( index,:)) /2;
        nback(x,y,:)  = (1+vec_back( index,:) ) /2;
        
        if(32==index)
            disp( [num2str(nroof(x,y,:) )]);
            disp( [num2str(nleft(x,y,:) )]);
            disp( [num2str(nfront(x,y,:) )]);
            disp( [num2str(nright(x,y,:) )]);
            disp( [num2str(nfloor(x,y,:) )]);
            disp( [num2str(nback(x,y,:) )]);
            disp( [' ']);
        end
        index = index+1;
    end
end

ntemp = nfront;
nroof = imrotate(nroof, 90); 
nleft = imrotate(nleft, -90);
nfront= imrotate(nback, 90);
nright= imrotate(nright, -90);
nfloor= imrotate(nfloor, 90);
nback = imrotate(ntemp, 90);


normals = [empty nroof empty;nleft nfront nright;empty nfloor empty; empty nback empty];
figure;imshow(normals);title('normals');
