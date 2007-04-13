function I = ReadPFM(fname)
%
% function I = ReadPFM(fname) reads the 
% High Dynamic Range image format .PFM taking 
% into account the number of channels and byte order.
%
% Jonas Unger, 2006

fid = fopen(fname, 'r');
%Read Header
line = fgetl(fid);
tmp=sscanf(line,'%c%c',2);
channels = 1;
if(tmp(2) == 'F')
    channels = 3;
end;

line = fgetl(fid);
dims = sscanf(line,'%d%d',2);

width = dims(1);
height = dims(2);

line = fgetl(fid);
tmp = sscanf(line,'%f',1);

byteorder = 'ieee-be';

if(tmp < 0)
    byteorder = 'ieee-le';
end;

%Read Data
data = fread(fid, width * height * channels,'float32', 0, byteorder);
fclose(fid);

% Get the channels
intl=(1:channels:width*height*channels);
R = reshape(data(intl), [width height]);
G = reshape(data(intl+1), [width height]);
B = reshape(data(intl+2), [width height]);
clear data;
I = zeros(height, width, channels);
I (:,:,1) = flipud(R');
I (:,:,2) = flipud(G');
I (:,:,3) = flipud(B');
