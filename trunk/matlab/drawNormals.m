function drawNormals( vec_roof,vec_left,vec_front,vec_right, vec_floor, vec_back, empty )

%calculate vectors for faces
index = 1;
%dx = 1.0/(facew);dy = 1.0/(faceh);
for y=1:32
    for x=1:32
        %disp(['(',num2str(y),',',num2str(x),')']);
        nroof(x,y,:)  = (1+vec_roof( index,:) )/2;
        nleft(x,y,:)  = (1+vec_left( index,:) )/2;
        nfront(x,y,:) = (1+vec_front( index,:))/2;
        nright(x,y,:) = (1+vec_right( index,:))/2;
        nfloor(x,y,:) = (1+vec_floor( index,:))/2;
        nback(x,y,:)  = (1+vec_back( index,:) )/2;
        index = index+1;
    end
end

normals = [empty nroof empty;nleft nfront nright;empty nfloor empty; empty nback empty];
figure;imshow(normals);title('normals');
