function uv_array = rotateVectors( x,y,z, vec_array )

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
    uv_array(i,:) =  VectorToCube( Rz*(Ry*(Rx*vec_array(i, :)')));
end
