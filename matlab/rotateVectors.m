function uv_array = rotateVectors( angle, vec_array )

%APPLY ROTATION
%around y axis
%R = [cos(angle) 0 sin(angle); 0 1 0; -sin(angle) 0 cos(angle)];
%Y?
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];
%R = [cos(angle) 0 -sin(angle); 0 1 0; sin(angle) 0 cos(angle)];

%Ry = [ cos(angle) 0 sin(angle); 0 1 0; -sin(angle) 0 cos(angle) ];
Rx = [1 0 0; 0 cos(angle) -sin(angle); 0 sin(angle) cos(angle) ];
%Rz = [ cos(angle) -sin(angle) 0;  sin(angle) cos(angle) 0;  0 0 1 ];

length = size( vec_array,1 );
for i=1:length
    uv_array(i,:) =  VectorToCube( Rx*vec_array(i, :)' );
end
