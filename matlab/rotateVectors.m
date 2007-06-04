function uv_array = rotateVectors( angle, vec_array )

%APPLY ROTATION
%around y axis
R = [cos(angle) 0 sin(angle); 0 1 0; -sin(angle) 0 cos(angle)];
length = size( vec_array,1 );
for i=1:length
    uv_array(i,:) =  VectorToCube( vec_array(i, :) * R );
end
