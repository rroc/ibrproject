function c=decompositionstep(c)

%disp(['2.1 size of c:', num2str(size(c)) ]);
temp=zeros(size(c));
limit=floor(size(c,2)/2);

%find differences between adjacent pixel values
for i=1:limit
     temp(1,i,1)         =(c(1,2*i-1,1) + c(1,2*i,1))/sqrt(2);
     temp(1,limit+i,1)   =(c(1,2*i-1,1)-c(1,2*i,1))/sqrt(2);

%     %red
%     temp(1,i,1)         =(c(1,2*i-1,1) + c(1,2*i,1))/sqrt(2);
%     temp(1,limit+i,1)   =(c(1,2*i-1,1)-c(1,2*i,1))/sqrt(2);
%     
%     %green
%     temp(1,i,2)=(c(1,2*i-1,2) + c(1,2*i,2))/sqrt(2);
%     temp(1,limit+i,2)=(c(1,2*i-1,2)-c(1,2*i,2))/sqrt(2);
%     
%     %blue
%     temp(1,i,3)=(c(1,2*i-1,3) + c(1,2*i,3))/sqrt(2);
%     temp(1,limit+i,3)=(c(1,2*i-1,3)-c(1,2*i,3))/sqrt(2);    
end

c=temp;
