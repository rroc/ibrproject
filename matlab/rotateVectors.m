function [uv_array normals] = rotateVectors( x,y,z, vec_array )

%APPLY ROTATION
%around y axis
%R = [cos(angle) 0 sin(angle); 0 1 0; -sin(angle) 0 cos(angle)];
%Y?
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];

Rx = [1 0 0; 0 cos(x) -sin(x); 0 sin(x) cos(x) ];
Ry = [ cos(y) 0 sin(y); 0 1 0; -sin(y) 0 cos(y) ];
Rz = [ cos(z) -sin(z) 0;  sin(z) cos(z) 0;  0 0 1 ];

length = size( vec_array,1 );
for i=1:length
    vec = Rz*(Ry*(Rx*vec_array(i, :)'));
%     length = sqrt( vec(1)*vec(1) + vec(2)*vec(2) +vec(3)*vec(3) );
%     vec = vec/length;
    uv_array(i,:) =  VectorToCube( vec );
    normals(i,:)  = vec;
end
